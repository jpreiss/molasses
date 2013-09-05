#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"
#include "triangle.h"
#include "rasterize.h"
#include "image.h"
#include "texture.h"
#include <iostream>

void toImage(Array2D<Vec> const &colors, Array2D<float> const &zbuffer, Image &img)
{
	for (int y = 0; y < colors.rows(); ++y)
	{
		for (int x = 0; x < colors.columns(); ++x)
		{
			if (fabs(zbuffer(y, x) - std::numeric_limits<float>::max()) < 1)
			{
				img(x, y) = ColorRGBA(0, 0, 0);
			}
			else
			{
				Vec col = 255 * colors(y, x);
				img(y, x) = ColorRGBA(
					clamp<unsigned char, float>(col.x),
					clamp<unsigned char, float>(col.y),
					clamp<unsigned char, float>(col.z));
			}
		}
	}
}

void textureMap(Array2D<Vec> const &coords, Array2D<ColorRGBA> const &texture, Array2D<float> const &zbuffer, Image &img)
{
	for (int y = 0; y < coords.rows(); ++y)
	{
		for (int x = 0; x < coords.columns(); ++x)
		{
			if (zbuffer(y, x) == std::numeric_limits<float>::max())
			{
				img(x, y) = ColorRGBA(0, 0, 0);
			}
			else
			{
				Vec coord = coords(y, x);
				//auto interp = bilinear(texture, coord[0], coord[1]);
				auto interp = nearestNeighbor(texture, coord[0], coord[1]);
				img(y, x) = interp;
			}
		}
	}
}


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
	Array2D<Vec> fragments(height, width);
	Image im(width, height);

	int counter = 0;
	sf::Clock clock;
	clock.restart();

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		im.fill();
		zbuffer.fill(std::numeric_limits<float>::max());

		cam.position = rot * cam.position;
		cam.direction = -cam.position.normalized();
		cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		double fov = radians(70);
		Mat projection = perspectiveProjection(cam, fov, 1, 100);
		Mat toScreen = Mat::identity();
		toScreen(0, 0) = height / 2;
		toScreen(1, 1) = height / 2;
		Mat shift = Mat::translation(Vec(width / 2, height / 2, 0));
		toScreen = shift * toScreen;

		for (int i = 0; i < 12; ++i)
		{
			int *tri = cubeTris + (3 * i);
			Triangle t(cubeVerts[tri[0]], cubeVerts[tri[1]], cubeVerts[tri[2]]);
			Vec coords[] = {
				texCoords[tri[0]],
				texCoords[tri[1]],
				texCoords[tri[2]] };

			t = projection * t;

			rasterize(t, coords, zbuffer, fragments, toScreen);
		}

		textureMap(fragments, tex, zbuffer, im);

		screen.update((sf::Uint8 const *)im.raw(), width, height, 0, 0);

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