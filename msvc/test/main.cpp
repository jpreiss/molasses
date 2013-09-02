#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"
#include "triangle.h"
#include "rasterize.h"

void rotateCube(sf::RenderWindow &window)
{
	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

	Vec cubeVerts[] = {
		Vec(-1, -1, -1),
		Vec( 1, -1, -1),
		Vec( 1,  1, -1),
		Vec(-1,  1, -1),
		Vec(-1, -1,  1),
		Vec( 1, -1,  1),
		Vec( 1,  1,  1),
		Vec(-1,  1,  1)
	};

	int cubeTris[] = {
		0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 7,
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		2, 6, 7,
		2, 7, 3,
		0, 4, 5,
		0, 5, 1
	};

	ColorRGBA faceColors[] = {
		ColorRGBA(255, 0, 0),
		ColorRGBA(0, 255, 0),
		ColorRGBA(0, 0, 255),
		ColorRGBA(255, 255, 0),
		ColorRGBA(0, 255, 255),
		ColorRGBA(255, 0, 255)
	};

	Line cubeLines[] = {
		Line(Vec(-1, -1, -1), Vec(1, -1, -1)),
		Line(Vec(1, -1, -1), Vec(1, 1, -1)),
		Line(Vec(1, 1, -1), Vec(-1, 1, -1)),
		Line(Vec(-1, 1, -1), Vec(-1, -1, -1)),

		Line(Vec(-1, -1, -1), Vec(-1, -1, 1)),
		Line(Vec(1, -1, -1), Vec(1, -1, 1)),
		Line(Vec(1, 1, -1), Vec(1, 1, 1)),
		Line(Vec(-1, 1, -1), Vec(-1, 1, 1)),

		Line(Vec(-1, -1, 1), Vec(1, -1, 1)),
		Line(Vec(1, -1, 1), Vec(1, 1, 1)),
		Line(Vec(1, 1, 1), Vec(-1, 1, 1)),
		Line(Vec(-1, 1, 1), Vec(-1, -1, 1)),
	};

	Quat rot = Quat::from_axis_angle(Vec(0, 0, 1), 0.005);

	Camera cam;
	cam.position = Vec(4, 0, 1.5);

	sf::Texture screen;
	screen.create(width, height);
	Image im(width, height);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	Array2D<float> zbuffer(width, height);

	int counter = 0;

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

			for (auto it = t.asVecs(); it < t.asVecs() + 3; ++it)
			{
				*it = projection * (*it);
				*it = *it / it->w;
				*it = toScreen * (*it);
			}

			rasterize(t, zbuffer, im, faceColors[i / 2]);
		}

		screen.update((sf::Uint8 const *)im.raw(), width, height, 0, 0);

		if (counter == 100)
		{
			screen.copyToImage().saveToFile("../../molasses.png");
		}

		window.clear();
        window.draw(sprite);
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