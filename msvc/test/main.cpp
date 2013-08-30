#include <SFML/Graphics.hpp>
#include "line2d.h"
#include "line.h"
#include "projection.h"
#include "math.h"
#include "quat.h"


void rotateCube(sf::RenderWindow &window)
{
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
	cam.position = Vec(2, 0, 0.5);

	sf::Texture screen;
	screen.create(640, 480);
	Image im(640, 480);
	sf::Sprite sprite;
	sprite.setTexture(screen);

	while (true)
	{
		im.initialize();
		cam.position = rot * cam.position;
		cam.direction = -cam.position.normalized();
		cam.up = Vec(0, 0, 1).normalTo(cam.direction).normalized();

		Mat projection = parallelProjection(cam, 100);

		for (int i = 0; i < 12; ++i)
		{
			Vec screenStart = projection * cubeLines[i].start + Vec(320, 240, 0);
			Vec screenEnd = projection * cubeLines[i].end + Vec(320, 240, 0);
			drawLine2D(screenStart, screenEnd, im, ColorRGBA(255.0, 255.0, 255.0));
		}

		screen.update((sf::Uint8 const *)im.raw(), 640, 480, 0, 0);

		window.clear();
        window.draw(sprite);
        window.display();
	}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");

	// screen.copyToImage().saveToFile("../../molasses.png");

    while (window.isOpen())
    {
		rotateCube(window);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}