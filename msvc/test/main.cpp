#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"
#include "rasterize.h"
#include "image.h"
#include "texture.h"
#include <iostream>
#include <functional>

void rotateCube(sf::RenderWindow &window)
{
	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

	// discontinuous faces
	Vec cubeVerts[] = {
		// 1 
		Vec(-1, -1, -1),
		Vec( 1, -1, -1),
		Vec( 1,  1, -1),
		Vec(-1,  1, -1),

		// 2
		Vec(1, -1, -1),
		Vec(1, -1, 1),
		Vec(1, 1, 1),
		Vec(1, 1, -1),

		// 3
		Vec(1, -1, 1),
		Vec(-1, -1, 1),
		Vec(-1, 1, 1),
		Vec(1, 1, 1),

		// 4
		Vec(-1, -1, 1),
		Vec(-1, -1, -1),
		Vec(-1, 1, -1),
		Vec(-1, 1, 1),

		// 5
		Vec(-1, 1, -1),
		Vec(1, 1, -1),
		Vec(1, 1, 1),
		Vec(-1, 1, 1),

		// 6
		Vec(1, -1, -1),
		Vec(-1, -1, -1),
		Vec(-1, -1, 1),
		Vec(1, -1, 1)
	};

	int cubeTris[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	Vec texCoords[] = {	
		// 1
		Vec(0, 16),
		Vec(8, 16),
		Vec(8, 8),
		Vec(0, 8),

		// 2
		Vec(8, 16),
		Vec(16, 16),
		Vec(16, 8),
		Vec(8, 8),
		
		// 3
		Vec(16, 16),
		Vec(24, 16),
		Vec(24, 8),
		Vec(16, 8),
				
		// 4
		Vec(24, 16),
		Vec(32, 16),
		Vec(32, 8),
		Vec(24, 8),

		// 5
		Vec(16, 8),
		Vec(24, 8),
		Vec(24, 0),
		Vec(16, 0),

		// 6
		Vec(16, 24),
		Vec(24, 24),
		Vec(24, 16),
		Vec(16, 16)
	};

	sf::Image texture;
	texture.loadFromFile("../../cube.png");
	auto raw = texture.getPixelsPtr();
	Array2D<ColorRGBA> tex(24, 32);
	std::copy(raw, raw + (4 * 32 * 24), (unsigned char *)tex.raw());

	Quat rot = Quat::from_axis_angle(Vec(0, 0, 1), 0.005);

	Camera cam;
	cam.position = Vec(4, 0, 1.5);

	sf::Texture screen;
	screen.create(width, height);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	Array2D<float> zbuffer(height, width);
	Array2D<ColorRGBA> fragments(height, width);

	int counter = 0;
	sf::Clock clock;
	clock.restart();

	auto cubeVertex = [](VertexIn const &in, VertexGlobal const &global) -> VertexIn
	{
		VertexIn out;
		out.coord = in.coord;
		out.normal = in.normal; // TODO
		out.vertex = global.modelViewProjection * in.vertex;
		return out;
	};

	auto cubeFragment = [&tex](VertexIn const &v) -> FragmentOut
	{
		FragmentOut f;
		f.color = nearestNeighbor(tex, v.coord.x, v.coord.y);
		return f;
	};

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

		cam.position = rot * cam.position;
		cam.direction = -cam.position.normalized();
		cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		double fov = radians(70);

		VertexGlobal global;
		global.modelView = view(cam);
		global.modelViewProjection = projection(fov, 1, 100) * view(cam);

		Mat toScreen = normalizedToScreen(width, height);

		for (int i = 0; i < 12; ++i)
		{
			int *tri = cubeTris + (3 * i);

			VertexIn vertices[3];

			for (int i = 0; i < 3; ++i)
			{
				vertices[i].vertex = cubeVerts[tri[i]];
				vertices[i].coord = texCoords[tri[i]];
				vertices[i] = cubeVertex(vertices[i], global);
			}

			rasterize(vertices[0], vertices[1], vertices[2], cubeFragment, zbuffer, fragments, toScreen);
		}

		screen.update((sf::Uint8 const *)fragments.raw(), width, height, 0, 0);

		if (counter == 100)
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