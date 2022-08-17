#include "Button.hpp"
class DragableButton : public Button {
public:
    DragableButton(ts::Rect* rect, ts::Text* text, int degreesOfFreedom, int initialStateIndex) : Button(rect, text) {
        this->degreesOfFreedom = degreesOfFreedom;
        this->state = initialStateIndex;

        createDragablePanel();
        createLines();
        createSlider();
        
        //difference between lineXPos calculation for lines of 1 and 0.
        spaceBetweenLines = calculateSpaceBetweenLines();
    }

private:
	void createDragablePanel() { 
        //hardcoded bounds for dragable rect
        int dragableXoffset = rect->getSize().x / 10;
        int dragableYoffset = rect->getSize().y / 2;
        int dragableWidth = rect->getSize().x - dragableXoffset;
        int dragableHeight = rect->getSize().y / 2;

        dragablePanel = (new ts::Rect(rect->getPosition().x + dragableXoffset,
            rect->getPosition().y + dragableYoffset, dragableWidth, dragableHeight))
            ->setColor(sf::Color(200, 200, 200));
    }

    void createLines() {
        barCount = degreesOfFreedom - 1;
        lineWidth = dragablePanel->getSize().x / (barCount * 5);// divide width by an extra number so that the rects do not touch
        dragableHeightByLineHeight = 0.8f;//factor between box height and line height
        lineHeight = dragablePanel->getSize().y * dragableHeightByLineHeight;
        lineXoffset = (rect->getPosition().x - dragablePanel->getPosition().x) / 2;
        lineYPos = dragablePanel->getPosition().y + (lineHeight * (1 - dragableHeightByLineHeight) / 2);
        for (int i = 0; i < degreesOfFreedom; i++) {
            float lineXPos = dragablePanel->getPosition().x + lineXoffset + ((i * dragablePanel->getSize().x) / barCount) - lineWidth / 2;
            lines.push_back((new ts::Rect(lineXPos, lineYPos, lineWidth, lineHeight))->setColor(sf::Color::Black));
        }
    }

    //create slider on default line (determined by initialStateIndex)
    void createSlider() {
        int sliderWidth = 10;
        float lineXPos = dragablePanel->getPosition().x + lineXoffset + ((state * dragablePanel->getSize().x) / barCount) - lineWidth / 2;
        //remove offset of line xPos (substracted above for readability) and add own offset
        float sliderXPos = lineXPos + lineWidth / 2 - sliderWidth / 2;
        slider = (new ts::Rect(sliderXPos, lineYPos, 10, lineHeight))->setColor(sf::Color::Black);
    }

    int calculateSpaceBetweenLines() {
		return  dragablePanel->getPosition().x
            + lineXoffset
            + ((dragablePanel->getSize().x) / barCount)
            - lineWidth / 2 // i = s1 
            - (dragablePanel->getPosition().x
            + lineXoffset - lineWidth / 2);//i = 0
    }

public:

    void update() override {
        if(wasPressed()) {
            mouseReleasedAfterPress = true;
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) == false) {
            mouseReleasedAfterPress = false;
        }
        if(mouseReleasedAfterPress == true) {
            changeSliderPosition(Mouse::getPosition(true));
        }
    }

    void changeSliderPosition(sf::Vector2i mousePos) {
        int sliderPos = slider->getPosition().x;
        while(mousePos.x < sliderPos - spaceBetweenLines + (lineWidth / 2)) {
            if(state > 0) {
                state --;
                sliderPos -= spaceBetweenLines;
            } else {
                break;
            }
        }
        while(mousePos.x > sliderPos + spaceBetweenLines - (lineWidth / 2)) {
            if(state < barCount) {
                state ++;
                sliderPos += spaceBetweenLines;
            } else {
                break;
            }
        }
        slider->transform(getSliderX(state), lineYPos);
    }

    /**
     * @brief Returns true if the Button was pressed since THE LAST CALL OF THIS FUNCTION
     * SIDE EFFECTS: Changes lastClickedCounter (counter used to distinguish different clicks)
     * @param renderer Needs the renderer and it's implemented mouse tracking functions
     * @return true 
     * @return false 
     */
    bool wasPressed() override {
        auto mousePos = Mouse::getLastLeftClick();
        auto size = slider->getSize();
        size.x *= 3;
        size.y *= 3;
        auto pos = slider->getPosition();
        pos.x -= size.x / 3;
        pos.y -= size.y / 3;

        if(mousePos.x > pos.x && mousePos.x < pos.x + size.x 
                && mousePos.y > pos.y && mousePos.y < pos.y + size.y
                && lastClickedCounter != Mouse::getClickCounter()) {
            lastClickedCounter = Mouse::getClickCounter();
            return true;
        }
        return false;
    }
    /*
    void draw() const override {
        renderer.draw(rect);
        renderer.draw(slider);
        renderer.drawText(text, fontName, rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y / 2, fontSize, textColor);

        for(sf::RectangleShape line : lines) {
            renderer.draw(line);
        }
    }
    */
    /**
     * @brief The relevant output to EventManager is just the current state of the slider.
     * 
     * @return vector<int> 
     */
    std::vector<int> getData() override {
        return {state};
    }

private:
    float getSliderX(int state) {
        float lineXPos = dragablePanel->getPosition().x + lineXoffset + ((state * dragablePanel->getSize().x) / barCount) - lineWidth / 2;
        //remove offset of line xPos (substracted above for readability) and add own offset
        float sliderXPos = lineXPos + lineWidth / 2 - slider->getSize().x / 2;
        return sliderXPos;
    }
    int state = -1;
    int degreesOfFreedom;

    ts::Rect* dragablePanel;
    ts::Rect* slider;
    std::vector<ts::Rect*> lines;

    bool mouseReleasedAfterPress = false;
    int barCount;
    float lineWidth, dragableHeightByLineHeight, lineHeight, lineXoffset, lineYPos;
    float spaceBetweenLines;
};