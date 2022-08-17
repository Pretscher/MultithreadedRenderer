#include "Rendering/Renderer.hpp"

/*we dont want the Renderer to have to know Events(and the flexibility to not use Events at all), so we use a void* instead of an Events::void pointer.
* Thus we have to pass an actual void which is declared here.*/
void callbackHelp();

class Events {
private:
    ts::Rect* rect;
    ts::Rect* rect2;
    ts::Circle* circle, * circle2;
    ts::Line* line;
    ts::Text* text;
public:
    Events() {
        //example drawings
        rect = (new ts::Rect(0, 0, 200, 200))->addTexture("Textures/container.jpg", true);//should not be overdrawn by second rect
        rect2 = (new ts::Rect(0, 100, 200, 200))->addOutline(sf::Color::Red, 10)->setColor(sf::Color::Blue);
        circle = (new ts::Circle(1000, 200, 50))->setColor(sf::Color::Green)->addOutline(sf::Color::Yellow, 5);
        circle2 = (new ts::Circle(500, 200, 50))->addTexture("Textures/awesomeface.png", true);
        line = (new ts::Line(1600, 0, 1600, 1080))->setColor(sf::Color::Yellow)->setThickness(10.0f);
        text = (new ts::Text(1680, 0, "A glorious Text!"))->setColor(sf::Color::Green);
        Renderer::addBackground("Textures/game-background-hills.jpg", true);//although called last, has priority 0 => is background
    }
   
    float x = 0.0f;
    bool goRight = true;
    void update() {
        if (goRight) {
            x++;
        }
        else {
            x--;
        }
        if (x >= 300) {
            goRight = false;
            //check if deleting a shape removes it from the renderer properly
            if (circle != nullptr) {
                delete circle;
                circle = nullptr;
            }

        }
        else if (x <= 10) {
            goRight = true;
        }
        //  circle->setRadius(x);
        circle2->setRadius(300 - x);
        rect->transform(x, 0);
        rect2->transform(x, 100);
    }
};

Events* events;
void callbackHelp() {
    events->update();
}

int main() {
    Renderer::init();
    events = new Events{};
    Renderer::startEventloop(&callbackHelp);
    return 0;
}