#pragma once
#include "Rendering/Drawables.hpp"

class ShapeBuilder {
protected:
    sf::Shape* shape;
public:
    ShapeBuilder() {}
    /**
     * @brief Draw this shape permanently (by adding it to the renderers list of permanent objects) and change it's properties through the pointer.
     * 
     * @return sf::Drawable* 
     */
    virtual ThreadSafeShape* build() {
        ThreadSafeShape* tsShape = new ThreadSafeShape(shape);
        Renderer::addPermanentObject(tsShape);
        return tsShape;
    }
    /**
     * @brief Get the shape without drawing or adding it to the renderer.
     * 
     * @return sf::Drawable* 
     */
    virtual ThreadSafeShape* getWithoutDrawing() {
        return new ThreadSafeShape(shape);
    }
    /**
     * @brief Set the color of this shape.
     * 
     * @param color 
     * @return ShapeBuilder& 
     */
    virtual ShapeBuilder& setColor(sf::Color color) {
        shape->setFillColor(color);
        return *this;
    }
    /**
     * @brief Add an outline for this shape.
     * 
     * @param color 
     * @return ShapeBuilder& 
     */
    virtual ShapeBuilder& addOutline(sf::Color color, int thickness) {
        shape->setOutlineColor(color);
        shape->setOutlineThickness(thickness);
        return *this;
    }

    /**
     * @brief Add a preexisting texture to this shape.
     * 
     * @param texture 
     * @return ShapeBuilder& 
     */
    virtual ShapeBuilder& addTexture(sf::Texture& texture) {
        shape->setTexture(&texture);
        if (texture.isRepeated() == true) {
            auto bounds = shape->getGlobalBounds();
            sf::IntRect rect = sf::IntRect(bounds.left, bounds.top, bounds.width, bounds.height);
            shape->setTextureRect(rect);
        }//else stretch
        return *this;
    }


    /**
     * @brief Load a texture and add it to this shape.
     * 
     * @param texturePath
     * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
     * @return ShapeBuilder& 
     */
    virtual ShapeBuilder& addTexture(std::string texturePath, bool repeat) {
        sf::Texture texture = loadTexture(texturePath, repeat);
        shape->setTexture(&texture);
        if (texture.isRepeated() == true) {
            auto bounds = shape->getGlobalBounds();
            sf::IntRect rect = sf::IntRect(bounds.left, bounds.top, bounds.width, bounds.height);
            shape->setTextureRect(rect);
        }//else stretch
        return *this;
    }
    
private:
    sf::Texture loadTexture(std::string path, bool repeat) {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
        {
            std::cout << "failed to load texture of path '" << path << "'";
            exit(0);
        }
        if (repeat == true) {
            sf::Image img = texture.copyToImage();
            if (texture.loadFromImage(img) == false) {
                std::cout << "failed to load texture image of path '" << path << "'";
                exit(0);
            }

            texture.setRepeated(true);
        }
        return texture;
    }
};