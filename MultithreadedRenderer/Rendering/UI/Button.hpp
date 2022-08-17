#pragma once
#include "Rendering/Renderer.hpp"

class Button {
public:
    Button(ts::Rect* rect, ts::Text* text) : rect(rect), text(text) {
        text->centerToRect(rect);
    }

    /**
     * @brief Call this with every iteration of the even thread in order to activate button functionality. No Functionality in base class.
     */
    virtual void update() {
        
    }
    /**
     * @brief Can be overridden to return some derived-class specific data. Base class just returns an empty vector.
     */
    virtual std::vector<int> getData() {
        return {};
    }

    /**
     * @brief Returns true if the Button is currently pressed.
     * 
     * @param renderer Needs the renderer and it's implemented mouse tracking functions
     * @return true 
     * @return false 
     */
    virtual bool isPressed() {
        auto mousePos = Mouse::getPosition(true);
        auto pos = rect->getPosition();
        auto size = rect->getSize();
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) 
            && mousePos.x > pos.x && mousePos.x < pos.x + size.x 
            && mousePos.y > pos.y && mousePos.y < pos.y + size.y
            ) {
            return true;
        }
        return false;
    }

    /**
     * @brief Returns true if the Button was pressed since THE LAST CALL OF THIS FUNCTION
     * SIDE EFFECTS: Changes lastClickedCounter (counter used to distinguish different clicks)
     * @param renderer Needs the renderer and it's implemented mouse tracking functions
     * @return true 
     * @return false 
     */
    virtual bool wasPressed() {
        auto mousePos = Mouse::getLastFinishedLeftClick();
        auto pos = rect->getPosition();
        auto size = rect->getSize();
        if(mousePos.x > pos.x && mousePos.x < pos.x + size.x 
                && mousePos.y > pos.y && mousePos.y < pos.y + size.y
                && lastClickedCounter != Mouse::getFinishedClickCounter()) {
            lastClickedCounter = Mouse::getFinishedClickCounter();
            return true;
        }
        return false;
    }
protected:
    int lastClickedCounter = -1;//click counter of last click that pressed this button
    ts::Rect* rect;
    ts::Text* text;
};