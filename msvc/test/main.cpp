#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"


void rotateCube(sf::RenderWindow &window)
{
	auto size = window.getSize();
	int width = size.x;
	int height = size.y;

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
	cam.position = Vec(3, 0, 0.5);

	sf::Texture screen;
	screen.create(width, height);
	Image im(width, height);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	int counter = 0;

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		im.initialize();
		cam.position = rot * cam.position;
		cam.direction = -cam.position.normalized();
		cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		double fov = radians(90);
		Mat projection = perspectiveProjection(cam, fov, 1, 100);
		Mat toScreen = Mat::identity();
		toScreen(0, 0) = height / 2;
		toScreen(1, 1) = height / 2;
		Mat shift = Mat::translation(Vec(width / 2, height / 2, 0));
		toScreen = shift * toScreen;

		for (int i = 0; i < 12; ++i)
		{
			Vec pstart = projection * cubeLines[i].start;
			pstart = pstart / pstart.w;
			Vec screenStart = toScreen * pstart;

			Vec pend = projection * cubeLines[i].end;
			pend = pend / pend.w;
			Vec screenEnd = toScreen * pend;

			drawLine2D(screenStart, screenEnd, im, ColorRGBA(255.0, 255.0, 255.0));
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
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Molasses works!");

	rotateCube(window);

    return 0;
}