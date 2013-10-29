#include <SFML/Graphics.hpp>
#include "line2d.h"
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
		return 8.0 / (last8_.front() - last8_.back()).asSeconds();
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
	Vec vertexUnprojected;
};

VertexWithUnprojected operator+(VertexWithUnprojected const &a, VertexWithUnprojected const &b)
{
	VertexWithUnprojected out;
	(VertexIn &)out = (VertexIn &)a + (VertexIn &)b;
	out.vertexUnprojected = a.vertexUnprojected + b.vertexUnprojected;
	return out;
}

VertexWithUnprojected operator*(float s, VertexWithUnprojected const &a)
{
	VertexWithUnprojected out;
	(VertexIn &)out = s * (VertexIn &)a;
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

void rotateCube(sf::RenderWindow &window)
{
	bool mouse = true;

	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

	sf::Image texture;
	texture.loadFromFile("../../../../models/cube.png");
	auto raw = texture.getPixelsPtr();
	Array2D<ColorRGBA> tex(24, 32);
	std::copy(raw, raw + (4 * 32 * 24), (unsigned char *)tex.raw());

	std::vector<Vec> verts;
	std::vector<int> tris;
	std::vector<Vec> normals;
	std::vector<int> triNormals;
	std::ifstream teapot("../../../../models/teapot.obj");
	readObj(teapot, verts, &normals, nullptr, tris, &triNormals, nullptr);

	// rectify teapot model
	Mat flip = Mat::fromRows33(Vec(1, 0, 0), Vec(0, 0, -1), Vec(0, 1, 0));

	Quat rot = Quat::from_axis_angle(Vec(0, 0, 1), 0.005);

	Camera cam;
	cam.position = Vec(30, 0, 5);

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
		//out.coord = in.coord;
		out.normal = global.modelView.withoutTranslation() * in.normal;
		out.vertex = global.modelViewProjection * in.vertex;
		out.vertexUnprojected = global.modelView * in.vertex;
		return out;
	};

	auto fragShade = [&](VertexWithUnprojected const &v, VertexGlobal const &global) -> FragmentOut
	{
		FragmentOut f;
		Vec eyeLight = global.view * light;
		Vec toLight = (eyeLight - v.vertexUnprojected).normalized();
		Vec normal = v.normal.normalized();
		float diffuse = clamp(dot(normal, toLight), 0.0f, 1.0f);

		Vec reflection = toLight.projectedTo(normal) - toLight.normalTo(normal);
		float specular = pow(clamp(-reflection.z, 0.0f, 1.0f), 8);

		float ambient = 0.1;

		f.color = 
			diffuse * ColorRGBA(20, 70, 150) +
			specular * ColorRGBA(100, 200, 255) +
			ambient * ColorRGBA(100, 150, 200);

		return f;
	};

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
        }

		zbuffer.fill(std::numeric_limits<float>::max());
		fragments.fill();

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
		global.modelView = view(cam) * flip;
		global.modelViewProjection = projection(fov, float(width)/height, 1, 100) * view(cam) * flip;
		global.normal = Mat::transpose(Mat::invert(global.modelView));

		Mat toScreen = normalizedToScreen(width, height);

		for (int i = 0; i < tris.size() / 3; ++i)
		{
			int *tri = tris.data() + (3 * i);
			int *triNorm = triNormals.data() + (3 * i);

			VertexIn vertices[3];

			VertexWithUnprojected shaded[3];

			for (int i = 0; i < 3; ++i)
			{
				vertices[i].vertex = verts[tri[i]];
				vertices[i].normal = normals[triNorm[i]];
				//vertices[i].coord = texCoords[tri[i]];
				shaded[i] = vertShade(vertices[i], global);
			}

			rasterize(shaded[0], shaded[1], shaded[2], fragShade, zbuffer, fragments, toScreen, global);
		}

		screen.update((sf::Uint8 const *)fragments.raw(), width, height, 0, 0);

		window.clear();
        window.draw(sprite);
        window.display();

		counter.frame();
		std::cout << counter.fps() << " FPS" << std::endl;
	}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "Molasses works!");
	sf::Mouse::setPosition(sf::Vector2i(320, 240), window);

	rotateCube(window);

    return 0;
}