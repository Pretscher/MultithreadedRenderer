#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"
namespace ts {
	class Drawable {
	protected:
		std::mutex mtx;
		sf::Drawable* drawable = nullptr;

		void initDrawableAfterConstruction(sf::Drawable* drawable);
	public:
		Drawable() {}
		Drawable(const Drawable& rect) = delete;
		Drawable(Drawable&& rect) = delete;
		~Drawable();
		
		//Prevents all changes to the drawable (transformations, resizings, recolorings etc.) until the lock is released with unlock(). 
		void lock() {
			mtx.lock();
		}

		//When called, all changes to the drawable are allowed again.
		void unlock() {
			mtx.unlock();
		}

		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced*/
		virtual void draw();

		void setPriority(int priority);
		
		int getPriority() {
			return priority;
		}
	private:
		int priority = 1;
	};

	class Shape : public Drawable {
	protected:
		sf::Shape* shape = nullptr;
		Shape() {};
		void initShapeAfterConstruction(sf::Shape* shape) {
			this->shape = shape;
			initDrawableAfterConstruction(shape);
		}

		void setColor(sf::Color color) {
			mtx.lock();
			shape->setFillColor(color);
			mtx.unlock();
		}
		/**
		 * @brief Add an outline for this shape.
		 *
		 * @param color
		 * @return Shape* this
		 */
		void addOutline(sf::Color color, float thickness) {
			mtx.lock();
			shape->setOutlineColor(color);
			shape->setOutlineThickness(thickness);
			mtx.unlock();
		}
		
		void transform(float x, float y) {
			mtx.lock();
			shape->setPosition(x, y);
			mtx.unlock();
		}
		/**
		 * @brief Load a texture and add it to this shape.
		 *
		 * @param texturePath
		 * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
		 * @return Shape* this
		 */
		void addTexture(std::string texturePath, bool repeat);
	};

	class Rect : public Shape {
	protected:
		sf::RectangleShape* rect;
	public:
		Rect() = delete;

		Rect(sf::RectangleShape* rect) : rect(rect) {
			this->rect = rect;
			initShapeAfterConstruction(rect);
		}
		
		Rect(float x, float y, float width, float height) : rect(new sf::RectangleShape(sf::Vector2f(width, height))) {
			rect->setPosition(x, y);
			initShapeAfterConstruction(rect);
		}


		//Builder functions generated from implementations in shape (done this way to avoid duplicate code)---------------------------------------------------

		
		Rect* transform(float x, float y) {
			Shape::transform(x, y);
			return this;
		}
		
		Rect* addOutline(sf::Color color, float thickness) {
			Shape::addOutline(color, thickness);
			return this;
		}
		
		Rect* setColor(sf::Color color) {
			Shape::setColor(color);
			return this;
		}
		
		Rect* resize(float width, float height) {
			mtx.lock();
			rect->setSize(sf::Vector2f(width, height));
			mtx.unlock();
			return this;
		}

		Rect* addTexture(std::string texturePath, bool repeat) {
			Shape::addTexture(texturePath, repeat);
			return this;
		}

		Rect* setPriority(int priority) {
			Drawable::setPriority(priority);
			return this;
		}
	};

	class Line : public Drawable {
	protected:
		sf::RectangleShape* line;
		float x2, y2;
	public:
		Line() = delete;

		Line(float x1, float y1, float x2, float y2) : line(new sf::RectangleShape()) {
			this->x2 = x2;
			this->y2 = y2;
			transform(x1, y1, x2, y2);
			
			initDrawableAfterConstruction(this->line);
		}

		Line* transformFirstPoint(float x1, float y1) {
			transform(x1, y1, this->x2, this->y2);
			return this;
		}

		Line* transformSecondPoint(float x2, float y2) {
			transform(line->getPosition().x, line->getPosition().y, x2, y2);
			return this;
		}

		Line* transform(float x1, float y1, float x2, float y2) {
			mtx.lock();
			float dX = x2 - x1;
			float dY = y2 - y1;

			float rot = atan2(dY, dX) * 57.2958f;
			line->setSize(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), line->getSize().y));
			line->setOrigin(0, line->getSize().y / 2);
			line->setRotation(rot);
			line->setPosition(x1, y1);
			mtx.unlock();
			return this;
		}

		Line* setColor(sf::Color color) {
			line->setFillColor(color);
			return this;
		}

		Line* setPriority(int priority) {
			Drawable::setPriority(priority);
			return this;
		}
	};

	class Circle : public Shape {
	protected:
		sf::CircleShape* circle;
	public:
		Circle() = delete;
		Circle(sf::CircleShape* circle) : circle(circle) {
			initShapeAfterConstruction(this->circle);
		}
		
		Circle(float x, float y, float radius) : circle(new sf::CircleShape(radius)) {
			circle->setPosition(x, y);
			initShapeAfterConstruction(this->circle);
		}

		Circle* setRadius(float radius) {
			mtx.lock();
			circle->setRadius(radius);
			mtx.unlock();
			return this;
		}

		Circle* transform(float x, float y) {
			Shape::transform(x, y);
			return this;
		}

		Circle* addOutline(sf::Color color, float thickness) {
			Shape::addOutline(color, thickness);
			return this;
		}

		Circle* setColor(sf::Color color) {
			Shape::setColor(color);
			return this;
		}

		Circle* addTexture(std::string texturePath, bool repeat) {
			Shape::addTexture(texturePath, repeat);
			return this;
		}

		Circle* setPriority(int priority) {
			Drawable::setPriority(priority);
			return this;
		}
	};

	class Text : public Drawable {
	protected:
		sf::Text* text;
	public:
		Text() = delete;
		Text(float x, float y, std::string string) : text(new sf::Text()) {
			text = new sf::Text();
			text->setPosition(x, y);
			text->setString(string);
			initDrawableAfterConstruction(text);
		}

		Text* transform(float x, float y) {
			text->setPosition(x, y);
			return this;
		}

		Text* setColor(sf::Color color) {
			mtx.lock();
			text->setFillColor(color);
			mtx.unlock();
			return this;
		}

		Text* setString(std::string string) {
			mtx.lock();
			text->setString(string);
			mtx.unlock();
			return this;
		}
		
		Text* setPriority(int priority) {
			Drawable::setPriority(priority);
			return this;
		}
	};

}