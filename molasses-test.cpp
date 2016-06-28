#include <SFML/Graphics.hpp>
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"
#include "rasterize.h"
#include "image.h"
#include "texture.h"
#include "objfile.h"
#include "bounds.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>

// isn't right for the first 8 frames
class FpsCounter
{
public:
	FpsCounter() :
		last8_(8)
	{
		clock_.restart();
	}

	void frame()
	{
		std::rotate(last8_.rbegin(), last8_.rbegin() + 1, last8_.rend());
		last8_[0] = clock_.getElapsedTime();
	}

	double fps()
	{
		return 7.0 / (last8_.front() - last8_.back()).asSeconds();
	}

	double sinceLast()
	{
		return (last8_[0] - last8_[1]).asSeconds();
	}

private:
	sf::Clock clock_;
	std::vector<sf::Time> last8_;
};

class VertexWithUnprojected : public VertexIn
{
public:
	Vec vertexWorld;
	Vec vertexUnprojected;
};

VertexWithUnprojected operator+(VertexWithUnprojected const &a, VertexWithUnprojected const &b)
{
	VertexWithUnprojected out;
	(VertexIn &)out = (VertexIn &)a + (VertexIn &)b;
	out.vertexWorld = a.vertexWorld + b.vertexWorld;
	out.vertexUnprojected = a.vertexUnprojected + b.vertexUnprojected;
	return out;
}

VertexWithUnprojected operator*(float s, VertexWithUnprojected const &a)
{
	VertexWithUnprojected out;
	(VertexIn &)out = s * (VertexIn &)a;
	out.vertexWorld = s * a.vertexWorld;
	out.vertexUnprojected = s * a.vertexUnprojected;
	return out;
}

Vec keyVelocity()
{
	using std::make_pair;

	std::pair<sf::Keyboard::Key, Vec> keyDirs[] = {
		make_pair(sf::Keyboard::W, Vec(0, 0, 1)),
		make_pair(sf::Keyboard::A, Vec(-1, 0, 0)),
		make_pair(sf::Keyboard::S, Vec(0, 0, -1)),
		make_pair(sf::Keyboard::D, Vec(1, 0, 0)),
		make_pair(sf::Keyboard::Space, Vec(0, 1, 0)),
		make_pair(sf::Keyboard::LControl, Vec(0, -1, 0)),
	};

	Vec velocity;

	for (int i = 0; i < sizeof(keyDirs) / sizeof(keyDirs[0]); ++i)
	{
		if (sf::Keyboard::isKeyPressed(keyDirs[i].first))
		{
			velocity = velocity + keyDirs[i].second;
		}
	}

	return velocity.normalized();
}

std::ostream &operator<<(std::ostream &s, Vec v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return s;
}

void printMatrix(std::ostream &s, Mat m)
{
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			s << std::setiosflags(std::ios::fixed)
				<< std::setprecision(3)
				<< std::setw(8);
			s << m(r, c);
		}
		s << "\n";
	}
}

void rotateCube(sf::RenderWindow &window)
{
	bool mouse = true;

	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

	/*
	sf::Image texture;
	texture.loadFromFile("models/cube.png");
	auto texRaw = texture.getPixelsPtr();
	Array2D<ColorRGBA> tex(24, 32);
	std::copy(texRaw, texRaw + (4 * 32 * 24), (unsigned char *)tex.data());
	*/

	std::vector<Vec> verts;
	std::vector<int> tris;
	std::vector<Vec> normals;
	std::vector<int> triNormals;
	std::ifstream teapot("models/teapot.obj");
	readObj(teapot, verts, &normals, nullptr, tris, &triNormals, nullptr);

	// rectify teapot model
	Mat teapotRectify = Mat::fromRows33(Vec(1, 0, 0), Vec(0, 0, -1), Vec(0, 1, 0));

	Bounds bds = Bounds::fromIterators(verts.begin(), verts.end());
	float roomSz = vabs(bds.size()).maxXYZEl();
	float floorZ = bds.mins.y; // rectified
	Vec floorVerts[] = {
		{roomSz, roomSz, floorZ, 1},
		{-roomSz, roomSz, floorZ, 1},
		{-roomSz, -roomSz, floorZ, 1},
		{roomSz, -roomSz, floorZ, 1},
	};
	int floorTris[] = {
		0, 1, 3,
		3, 1, 2,
	};

	Camera cam;
	cam.position = Vec(30, 14, 5);

	// replace angle calc with mouse once it's ready
	cam.direction = -cam.position.normalized();
	cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

	sf::Texture screen;
	screen.create(width, height);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	Array2D<float> zbuffer(height, width);
	Array2D<ColorRGBA> fragments(height, width);


	FpsCounter counter;

	Vec light = Vec(0, 30, 50);

	auto vertShade = [](VertexIn const &in, VertexGlobal const &global) -> VertexWithUnprojected
	{
		VertexWithUnprojected out;
		out.coord = in.coord;
		out.normal = global.modelView.withoutTranslation() * in.normal;
		out.vertex = global.modelViewProjection * in.vertex;
		out.vertexUnprojected = global.modelView * in.vertex;
		return out;
	};

	ColorRGBA red   { 255,   0,   0 };
	ColorRGBA green {   0, 255,   0 };

	ColorRGBA mixed;
	float mix = 0;

	auto fragShadeShiny = [&](VertexWithUnprojected const &v, VertexGlobal const &global) -> FragmentOut
	{
		FragmentOut f;
		Vec eyeLight = global.view * light;
		Vec toLight = (eyeLight - v.vertexUnprojected).normalized();
		//Vec normal = v.normal.normalized();
		Vec normal = v.normal;
		float diffuse = clamp(dot(normal, toLight), 0.0f, 1.0f);

		Vec reflection = toLight.projectedTo(normal) - toLight.normalTo(normal);
		float specular = clamp(-reflection.z, 0.0f, 1.0f);
		specular *= specular;
		specular *= specular;

		float ambient = 0.3;

		f.color = 
			diffuse * mixed +
			specular * ColorRGBA(100, 200, 255) +
			ambient * ColorRGBA(100, 150, 200);

		//f.color = mixed;

		return f;
	};
	
	auto fragShadeChecker = [&](VertexWithUnprojected const &v, VertexGlobal const &global) -> FragmentOut
	{
		static int i = 0;
		FragmentOut f;
		float SCALE = 0.2;
		int xx = 10000 + SCALE * v.coord.x;
		int yy = 10000 + SCALE * v.coord.y;
		if ((xx & 1) != (yy & 1)) {
			f.color = ColorRGBA(0, 0, 0);
		}
		else {
			f.color = ColorRGBA(0, 0x99 , 0xFF);
		}
		return f;
	};
	// Declare and load a font
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		std::cout << "failed to load font\n";
		return;
	}

	// Create a text
	sf::Text text("hello", font);
	text.setCharacterSize(16);
	text.setColor(sf::Color::Red);
	text.setPosition(10, 10);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::P)
				{
					screen.copyToImage().saveToFile("../../molasses.png");
				}
				if (event.key.code == sf::Keyboard::Q)
				{
					window.close();
				}
				if (event.key.code == sf::Keyboard::M)
				{
					mouse = !mouse;
				}
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				mix = 1.0;
			}
		}

		std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<float>::max());
		std::fill(fragments.begin(), fragments.end(), ColorRGBA());

		Mat camCsys = Mat::transpose(view(cam).withoutTranslation());

		double speed = counter.sinceLast() * 80;

		Vec v = camCsys * keyVelocity();
		cam.position = cam.position + speed * v;

		window.setMouseCursorVisible(!mouse);
		if (mouse)
		{
			auto mouse = sf::Mouse::getPosition(window);
			auto size = window.getSize();
			auto middle = sf::Vector2i(size.x / 2, size.y / 2);
			auto dmouse = sf::Vector2i(mouse.x - middle.x, mouse.y - middle.y);

			dmouse.y *= -1;
			sf::Mouse::setPosition(middle, window);

			Vec mouseVelocity = Vec(dmouse.x, dmouse.y, 0) / 300.0;
			Vec worldTurn = camCsys * mouseVelocity;
			cam.direction = (cam.direction + worldTurn).normalized();
			cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized(); // not most efficient
		}

		//cam.position = rot * cam.position;
		//cam.direction = -cam.position.normalized();
		//cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		double fov = radians(60);

		VertexGlobal global;
		global.view = view(cam);
		global.modelView = view(cam) * teapotRectify;
		global.modelViewProjection = projection(fov, float(width)/height, 10, 100) * global.modelView;
		global.normal = Mat::transpose(Mat::invert(global.modelView));

		Mat toScreen = normalizedToScreen(width, height);

		for (int i = 0; i < tris.size() / 3; ++i)
		{
			int *tri = tris.data() + (3 * i);
			int *triNorm = triNormals.data() + (3 * i);

			VertexIn vertices[3];

			VertexWithUnprojected shaded[3];

			for (int j = 0; j < 3; ++j)
			{
				vertices[j].vertex = verts[tri[j]];
				vertices[j].normal = normals[triNorm[j]];
				//vertices[j].coord = texCoords[tri[j]];
				shaded[j] = vertShade(vertices[j], global);
			}

			rasterize(shaded[0], shaded[1], shaded[2], fragShadeShiny, zbuffer, fragments, toScreen, global);
		}

		global.view = view(cam);
		global.modelView = view(cam);
		global.modelViewProjection = projection(fov, float(width)/height, 1, 100) * global.modelView;
		global.normal = Mat::transpose(Mat::invert(global.modelView));

		for (int i = 0; i < 2; ++i)
		{
			int *tri = floorTris + 3 * i;

			VertexIn vertices[3];

			VertexWithUnprojected shaded[3];

			for (int j = 0; j < 3; ++j)
			{
				vertices[j].vertex = floorVerts[tri[j]];
				vertices[j].normal = {0, 0, 1};
				vertices[j].coord = {vertices[j].vertex};
				vertices[j].coord.z = 0;
				vertices[j].coord.w = 0;
				//std::cout << vertices[j].coord;

				shaded[j] = vertShade(vertices[j], global);
			}

			rasterize(shaded[0], shaded[1], shaded[2], fragShadeChecker, zbuffer, fragments, toScreen, global);
		}

		screen.update((sf::Uint8 const *)fragments.data(), width, height, 0, 0);

		counter.frame();
		std::ostringstream oss;
		oss << (int)counter.fps() << " FPS" << std::endl;
		text.setString(oss.str());

		window.clear();
		window.draw(sprite);
		window.draw(text);
		window.display();

		mix -= 2.0 * counter.sinceLast() * mix;
		mixed = mix * green + (1.0 - mix) * red;
	}
}

int main()
{
	auto modes = sf::VideoMode::getFullscreenModes();
	auto mode = modes[0];

	// if retina screen, find the half-resolution, they are too big to render fast
	if (mode.width > 1920) {
		for (auto m = modes.begin() + 1; m != modes.end(); ++m) {
			if (m->width == mode.width / 2 && m->height == mode.height / 2) {
				mode = *m;
			}
		}
	}

	sf::RenderWindow window(mode, "Molasses works!", sf::Style::Fullscreen);
	window.setVerticalSyncEnabled(true);

	sf::Mouse::setPosition(sf::Vector2i(mode.width / 2, mode.height / 2), window);

	rotateCube(window);

	return 0;
}
