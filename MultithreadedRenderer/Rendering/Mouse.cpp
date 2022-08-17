#include "Mouse.hpp"
#include "Renderer.hpp"

sf::Vector2i Mouse::lastClickPos, Mouse::lastFinishedClickPos;
sf::Vector2i Mouse::lastMousePos;//for if the mouse leaves the window
bool Mouse::clickComplete;
int Mouse::clickCounter;
int Mouse::finishedClickCounter;

sf::Vector2i Mouse::getPosition(bool factorInBorders) {
    auto pos = sf::Mouse::getPosition(*Renderer::window);
    int xPixels = Renderer::getPixelCountX();
    int yPixels = Renderer::getPixelCountY();
    //although SFML handles resizing for us, it somehow does not handle the mouse position after a resize properly.
    pos.x = xPixels * pos.x / Renderer::window->getSize().x;//xPixels is the initial size of the window, which is used by SFML to scale the window.
    pos.y = yPixels * pos.y / Renderer::window->getSize().y;

    if (factorInBorders == true) {
        if (pos.x < xPixels && pos.y < yPixels) {
            lastMousePos = sf::Vector2i(pos.x, pos.y);//if the mouse leaves the window, the last mouse pos that was called with "factorInBordes = true" will be returned
            return sf::Vector2i(pos.x, pos.y);
        }
        else {
            return lastMousePos;
        }
    }
    else {
        return sf::Vector2i(pos.x, pos.y);
    }
}