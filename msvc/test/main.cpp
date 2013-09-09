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
	Vec velocity;

	sf::Keyboard::Key keys[] = 
		{ sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D };

	Vec directions[] =
		{ Vec(0, 0, 1), Vec(-1, 0, 0), Vec(0, 0, -1), Vec(1, 0, 0) };

	for (int i = 0; i < 4; ++i)
	{
		if (sf::Keyboard::isKeyPressed(keys[i]))
		{
			velocity = velocity + directions[i];
		}
	}

	return velocity.normalized();
}

void rotateCube(sf::RenderWindow &window)
{
	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

	sf::Image texture;
	texture.loadFromFile("../../cube.png");
	auto raw = texture.getPixelsPtr();
	Array2D<ColorRGBA> tex(24, 32);
	std::copy(raw, raw + (4 * 32 * 24), (unsigned char *)tex.raw());

	std::vector<Vec> verts;
	std::vector<int> tris;
	std::vector<Vec> normals;
	std::vector<int> triNormals;
	std::ifstream teapot("../../teapot.obj");
	readObj(teapot, verts, &normals, nullptr, tris, &triNormals, nullptr);

	// rectify teapot model
	Mat flip = Mat::fromRows33(Vec(1, 0, 0), Vec(0, 0, -1), Vec(0, 1, 0));

	Quat rot = Quat::from_axis_angle(Vec(0, 0, 1), 0.005);

	Camera cam;
	cam.position = Vec(30, 0, 0);
	cam.direction = Vec(-1, 0, 0);
	cam.up = Vec(0, 0, 1);

	sf::Texture screen;
	screen.create(width, height);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	Array2D<float> zbuffer(height, width);
	Array2D<ColorRGBA> fragments(height, width);

	int counter = 0;
	sf::Clock clock;
	clock.restart();

	Vec light = Vec(0, 30, 100);

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
		float dp = dot(v.normal.normalized(), toLight);
		//f.color = nearestNeighbor(tex, v.coord.x, v.coord.y);
		f.color = (clamp(dp / 2, 0.0f, 1.0f) * ColorRGBA(255, 255, 0));
		f.color = f.color + 0.4 * ColorRGBA(200, 150, 50);
		return f;
	};

	auto prevMouse = sf::Mouse::getPosition(window);

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		zbuffer.fill(std::numeric_limits<float>::max());
		fragments.fill();

		Vec v = Mat::transpose(view(cam).withoutTranslation()) * keyVelocity();
		cam.position = cam.position + 0.3 * v;

		auto mouse = sf::Mouse::getPosition(window);
		auto mouseDelta = mouse - prevMouse;


		//cam.position = rot * cam.position;
		//cam.direction = -cam.position.normalized();
		//cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		double fov = radians(60);

		VertexGlobal global;
		global.view = view(cam);
		global.modelView = view(cam) * flip;
		global.modelViewProjection = projection(fov, 1, 100) * view(cam) * flip;
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

		if (counter == 230)
		{
			screen.copyToImage().saveToFile("../../molasses.png");
		}

		window.clear();
        window.draw(sprite);

		double fps = ((double) counter) / clock.getElapsedTime().asSeconds();
		std::cout << fps << " FPS" << std::endl;

        window.display();

		++counter;
	}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "Molasses works!");

	rotateCube(window);

    return 0;
}