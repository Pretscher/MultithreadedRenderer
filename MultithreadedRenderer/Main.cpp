#include "Rendering/Renderer.hpp"
#include "Rendering/Mouse.hpp"
#include <iostream>

#include "ClockThread.hpp"
using namespace std;

ts::Rect* rect;
ts::Rect* rect2;
ts::Circle *circle, *circle2;
ts::Line *line;
ts::Text *text;

void init() {
    rect = (new ts::Rect(0, 0, 200, 200))->addTexture("myRecources/Textures/container.jpg", true)->setPriority(2);//should not be overdrawn by second rect
    rect2 = (new ts::Rect(0, 100, 200, 200))->addOutline(sf::Color::Red, 10)->setColor(sf::Color::Blue);
    circle = (new ts::Circle(1000, 200, 50))->setColor(sf::Color::Green)->addOutline(sf::Color::Yellow, 5);
    circle2 = (new ts::Circle(500, 200, 50))->addTexture("myRecources/Textures/awesomeface.png", true);
    line = (new ts::Line(1600, 0, 1600, 1080))->setColor(sf::Color::Red);
    text = (new ts::Text(1600, 0, "A glorious Text!"))->setColor(sf::Color::Green);

    Renderer::addBackground("myRecources/Textures/game-background-hills.jpg", true);//although called last, has priority 0 => is background
}

float x;
bool goRight;
void eventloop() {
    if(goRight) {
        x++;
    } else {
        x--;
    }
    if(x >= 300) {
        goRight = false;
		//check if deleting a shape removes it from the renderer properly
        if (circle != nullptr) {
            delete circle;
            circle = nullptr;
        }

    } else if(x <= 10) {
        goRight = true;
    }
  //  circle->setRadius(x);
    circle2->setRadius(300 - x);
    rect->transform(x, 0);
    rect2->transform(x, 100);
}

int main() {
    bool initDone = false;

  //  ClockThread clock;
    Renderer::initWindow();
    sf::Event events;
    while(Renderer::window->isOpen()) {
      //  this_thread::sleep_for(1ms);
        while (Renderer::window->pollEvent(events)) {
            if (events.type == sf::Event::Closed) {
                Renderer::window->close();
                break;
            }
        }

        if(!initDone) {//we want to poll events before initialising once to avoid bugs
            initDone = true;
            init();
        }

        eventloop();
        Mouse::update();
    }
    Renderer::joinDrawingThread();
    return 0;
}