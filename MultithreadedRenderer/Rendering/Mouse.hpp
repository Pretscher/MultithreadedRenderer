#pragma once
#include "SFML/Graphics.hpp"

class Mouse {
public:
    Mouse() = delete;

    static sf::Vector2i getMousePos(bool factorInBorders);

    static sf::Vector2i getLastFinishedLeftClick() {
        return lastFinishedClickPos;
    }
    static sf::Vector2i getLastLeftClick() {
        return lastClickPos;
    }
    static int getClickCounter() {
        return clickCounter;
    }
    /**
     * @brief Every external object that wants to only be clicked once per mouseclick has to save the last finished clickCounter
     * @param clickCounter
     */
    static int getFinishedClickCounter() {
        return finishedClickCounter;
    }

    static void update() {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == true && clickComplete == true) { //mouse currently pressed => current click is not complete
            finishedClickCounter = false;
            clickComplete = false;
            clickCounter++;
            lastClickPos = getMousePos(true);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == false && clickComplete == false) {
            clickComplete = true;
            finishedClickCounter++;
            lastFinishedClickPos = getMousePos(true);//save mouse coords to last click positions
        }
    }

protected:
    static sf::Vector2i lastClickPos, lastFinishedClickPos;
    static sf::Vector2i lastMousePos;//for if the mouse leaves the window
    static bool clickComplete;
    static int clickCounter;
    static int finishedClickCounter;
};