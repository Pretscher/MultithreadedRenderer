#include "Rendering/Renderer.hpp"
#include "Rendering/ObjectBuilders.hpp"
#include "Rendering/Mouse.hpp"
#include <iostream>

#include "ClockThread.hpp"
using namespace std;

ts::Shape *rect, *rect2;
ts::Shape* circle;
ts::Text *text;

void init() {
    rect = RectBuilder(0, 0, 100, 100).setColor(sf::Color::Red).addOutline(sf::Color::Blue, 10).build();
    rect2 = RectBuilder(0, 0, 100, 100).setColor(sf::Color::Blue).addOutline(sf::Color::Red, 10).build();
    circle = CircleBuilder(500, 200, 50).setColor(sf::Color::Green).addOutline(sf::Color::Yellow, 10).build();
    text = TextBuilder(500, 500).setString("Hello world!").setColor(sf::Color::White).build();
}

int x;
bool circleDeleted = false;
bool goRight;
void eventloop() {
    if(goRight) {
        x++;
    } else {
        x--;
    }
    if(x >= 1000) {
        goRight = false;
        if (circleDeleted == false) {
            delete circle;
            circle = nullptr;
			circleDeleted = true;
        }
    } else if(x <= 10) {
        goRight = true;
    }
    rect->transform(x, 10);
    rect2->transform(x, 130);

}

int main() {
    bool initDone = false;

  //  ClockThread clock;
    Renderer::initWindow();
    sf::Event events;
    while(Renderer::window->isOpen()) {
        //this_thread::sleep_for(1ms);
        while (Renderer::window->pollEvent(events)) {
            if (events.type == sf::Event::Closed) {
                Renderer::window->close();
                return 0;
            }
        }

        if(!initDone) {//we want to poll events before initialising once to avoid bugs
            initDone = true;
            init();
        }

        eventloop();
        Renderer::drawNextFrame();
        Mouse::update();
    }
    Renderer::joinDrawingThread();
    Renderer::freeAllMemory();
    return 0;
}