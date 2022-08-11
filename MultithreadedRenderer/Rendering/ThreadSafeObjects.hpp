#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"
namespace ts {
	class Drawable {
	protected:
		std::mutex* mtx;
		sf::Drawable* drawable = nullptr;

		void initDrawableAfterConstruction(sf::Drawable* drawable);
	public:
		Drawable() {
			mtx = new std::mutex();
		}
		//Because of moving and copying freeMemory() has to be called to actually delete the pointers. This just nulls them.
		~Drawable() {
			mtx = nullptr;
			drawable = nullptr;
		}
		//Deletes the pointers.
		void freeMemory() {
			delete mtx;
			delete drawable;
		}
		
		Drawable(const Drawable& rect) {
			mtx = rect.mtx;
			drawable = rect.drawable;
		}
		
		//will not delete the pointers.
		Drawable(Drawable&& rect);

		//Prevents all changes to the drawable (transformations, resizings, recolorings etc.) until the lock is released with unlock(). 
		void lock() {
			mtx->lock();
		}

		//When called, all changes to the drawable are allowed again.
		void unlock() {
			if (mtx->try_lock() == false) {
				mtx->unlock();
			}
		}
		
		Drawable& operator = (const Drawable& toCopy) {
			Drawable copy;
			copy.mtx = toCopy.mtx;
			copy.drawable = toCopy.drawable;
			return copy;
		}
		
		bool operator == (const Drawable& toCompare) const {
			return drawable == toCompare.drawable;
		}

		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced*/
		virtual void draw();
	};

	class Shape : public Drawable {
	protected:
		sf::Shape* shape = nullptr;
	public:
		Shape() {}

		void initShapeAfterConstruction(sf::Shape* shape) {
			this->shape = shape;
			initDrawableAfterConstruction(shape);
		}

		void setColor(sf::Color color) {
			shape->setFillColor(color);
		}
		/**
		 * @brief Add an outline for this shape.
		 *
		 * @param color
		 * @return Shape* this
		 */
		void addOutline(sf::Color color, float thickness) {
			shape->setOutlineColor(color);
			shape->setOutlineThickness(thickness);
		}
		
		Shape& transform(float x, float y) {
			mtx->lock();
			shape->setPosition(x, y);
			mtx->unlock();
			return *this;
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
		Rect() {}

		Rect(sf::RectangleShape* rect) {
			this->rect = rect;
			initShapeAfterConstruction(rect);
		}
		
		Rect(int x, int y, int width, int height) : rect(new sf::RectangleShape(sf::Vector2f(width, height))) {
			rect->setPosition(x, y);
			initShapeAfterConstruction(rect);
		}

		Rect& resize(float width, float height) {
			mtx->lock();
			rect->setSize(sf::Vector2f(width, height));
			mtx->unlock();
			return *this;
		}
	};

	class Line : public Drawable {
	protected:
		sf::RectangleShape* line;
		float x2, y2;
	public:
		Line() {}

		Line(float x1, float y1, float x2, float y2) {
			this->x2 = x2;
			this->y2 = y2;
			line = new sf::RectangleShape();
			transform(x1, y1, x2, y2);
			
			initDrawableAfterConstruction(this->line);
		}

		void transformFirstPoint(float x1, float y1) {
			transform(x1, y1, this->x2, this->y2);
		}

		void transformSecondPoint(float x2, float y2) {
			transform(line->getPosition().x, line->getPosition().y, x2, y2);
		}

		void transform(float x1, float y1, float x2, float y2) {
			mtx->lock();
			float dX = x2 - x1;
			float dY = y2 - y1;

			float rot = atan2(dY, dX) * 57.2958f;
			line->setSize(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), line->getSize().y));
			line->setOrigin(0, line->getSize().y / 2);
			line->setRotation(rot);
			line->setPosition(x1, y1);
			mtx->unlock();
		}

		void setColor(sf::Color color) {
			line->setFillColor(color);
		}
	};

	class Circle : public Shape {
	protected:
		sf::CircleShape* circle;
	public:
		Circle() {};
		Circle(sf::CircleShape* circle) {
			this->circle = circle;
			initShapeAfterConstruction(this->circle);
		}
		
		Circle(int x, int y, int radius) {
			circle = new sf::CircleShape(radius);
			circle->setPosition(x, y);
			initShapeAfterConstruction(this->circle);
		}

		void setRadius(float radius) {
			mtx->lock();
			circle->setRadius(radius);
			mtx->unlock();
		}
	};

	class Text : public Drawable {
	protected:
		sf::Text* text;
	public:
		Text() {}
		Text(sf::Text* text) {
			this->text = text;
			initDrawableAfterConstruction(text);
		}


		void transform(float x, float y) {
			mtx->lock();
			text->setPosition(x, y);
			mtx->unlock();
		}
	};

}