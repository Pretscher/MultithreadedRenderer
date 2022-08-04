#pragma once
#include "ShapeBuilder.hpp"
class RectBuilder : public ShapeBuilder {
public:
    RectBuilder(int x, int y, int width, int height) {
        shape = new sf::RectangleShape(sf::Vector2f(width, height));
        shape->setPosition(x, y);
    }
};