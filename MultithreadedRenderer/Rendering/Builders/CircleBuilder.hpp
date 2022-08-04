#pragma once
#include "ShapeBuilder.hpp"
class CircleBuilder : public ShapeBuilder {
public:
    CircleBuilder(int x, int y, int radius) {
        shape = new sf::CircleShape(radius);
        shape->setPosition(x, y);
    }
};