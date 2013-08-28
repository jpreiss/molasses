#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"


int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");

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

	Camera cam;
	cam.position = Vec(5, 3, 8);
	cam.direction = -cam.position.normalized();
	cam.up = Vec(1, 0.2, 0).normalTo(cam.direction).normalized();

	Mat projection = parallelProjection(cam, 100);

	sf::Texture screen;
	screen.create(640, 480);

	Image im(640, 480);

	for (int i = 0; i < 12; ++i)
	{
		Vec screenStart = projection * cubeLines[i].start + Vec(320, 240, 0);
		Vec screenEnd = projection * cubeLines[i].end + Vec(320, 240, 0);
		drawLine2D(screenStart, screenEnd, im, ColorRGBA(255.0, 255.0, 255.0));
	}

	screen.update((sf::Uint8 const *)im.raw(), 640, 480, 0, 0);

	screen.copyToImage().saveToFile("../../molasses.png");

	sf::Sprite sprite;
	sprite.setTexture(screen);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}