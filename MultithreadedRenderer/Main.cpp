#include "Rendering/Renderer.hpp"
#include "Rendering/ObjectBuilders.hpp"
#include "Rendering/Mouse.hpp"
#include <iostream>

#include "ClockThread.hpp"
using namespace std;

ts::Rect *rect, *rect2;
ts::Circle *circle;
ts::Line* line;
ts::Text *text;

void init() {
    rect = RectBuilder(0, 0, 100, 100).addTexture("myRecources/Textures/container.jpg", true).build();
    rect2 = RectBuilder(0, 0, 100, 100).setColor(sf::Color::Blue).build();
    circle = CircleBuilder(500, 200, 50).addTexture("myRecources/Textures/awesomeface.png", true).build();
    text = TextBuilder(0, 350).setString("A glorious Text!").setColor(sf::Color::White).build();
    line = LineBuilder(0, 400, 1000, 400).setColor(sf::Color::White).build();
    Renderer::addBackground("myRecources/Textures/game-background-hills.jpg", true);
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
    } else if(x <= 10) {
        goRight = true;
    }
    circle->setRadius(x);
    rect->transform(x, 0);
    rect2->transform(x, 100);
    line->transformFirstPoint(x, x);
}

int main() {
    bool initDone = false;

  //  ClockThread clock;
    Renderer::initWindow();
    sf::Event events;
    while(Renderer::window->isOpen()) {
        this_thread::sleep_for(1ms);
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
        Mouse::update();
    }
    Renderer::joinDrawingThread();
    Renderer::freeAllMemory();
    return 0;
}