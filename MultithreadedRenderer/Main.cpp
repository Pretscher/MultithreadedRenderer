#include "Rendering/Renderer.hpp"
#include "Rendering/Mouse.hpp"
#include <iostream>

#include "ClockThread.hpp"
using namespace std;

ts::Rect rect;
ts::Rect rect2;
ts::Circle circle, circle2;
ts::Line line;
ts::Text text;

void init() {
    rect = ts::Rect(0, 0, 200, 200); rect.addTexture("myRecources/Textures/container.jpg", true);
    rect2 = ts::Rect(0, 210, 200, 200); rect2.addOutline(sf::Color::Red, 10); rect2.setColor(sf::Color::Blue);
    circle = ts::Circle(1000, 200, 50); circle.setColor(sf::Color::Green); circle.addOutline(sf::Color::Yellow, 5);
    //circle2 = ts::Circle(0, 0, 200, 200); rect.addTexture("myRecources/Textures/container.jpg", true);
    line = ts::Line(1600, 0, 1600, 1080); line.setColor(sf::Color::Red);
  //  text = ts::Text(0, 0, 200, 200); rect.addTexture("myRecources/Textures/container.jpg", true);
   
    // rect = RectBuilder(0, 0, 100, 100).addTexture("myRecources/Textures/container.jpg", true).build();
    //rect2 = RectBuilder(0, 0, 100, 100).build();
    //circle2 = CircleBuilder(500, 200, 50).addTexture("myRecources/Textures/awesomeface.png", true).build();
    //circle = CircleBuilder(1000, 200, 50).setColor(sf::Color::Green).addOutline(sf::Color::Yellow, 10).build();
    //text = TextBuilder(1600, 0).setString("A glorious Text!").setColor(sf::Color::Green).build();
    //line = LineBuilder(1600, 0, 1600, 1080).setColor(sf::Color::Black).setThickness(5).build();
  //  Renderer::addBackground("myRecources/Textures/game-background-hills.jpg", true);
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
    circle.setRadius(x);
    //circle2->setRadius(300 - x);
    rect.transform(x, 0);
    rect2.transform(x, 210);
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