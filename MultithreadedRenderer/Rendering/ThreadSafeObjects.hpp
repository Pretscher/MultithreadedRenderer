#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"

namespace ts {
	class Drawable {
	protected:
		std::mutex mtx{};
		sf::Drawable* drawable;
	public:
		Drawable(sf::Drawable* drawable) : drawable(drawable) {}
		~Drawable();//implemented in the cpp file because it has to inform the renderer of the deletion, and the renderer includes this header.
		void lock() {
			mtx.lock();
		}

		void unlock() {
			if (mtx.try_lock() == false) {
				mtx.unlock();
			}
		}

		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced*/
		virtual void draw();
		
	protected:
		//used in renderer map 
		std::string texturePath;
	public:

		void setTexturePath(std::string path) {
			texturePath = path;
		}
		
		bool hasTexturePath() {
			return texturePath.size() > 0;
		}
		std::string getTexturePath() {
			return texturePath;
		}
	};

class Rect : public Drawable {
	protected:
		sf::RectangleShape* rect;
	public:
		Rect(sf::RectangleShape* rect) : Drawable(rect) {
			this->rect = rect;
		}

		~Rect() {
			delete rect;
		}

		void transform(float x, float y) {
			mtx.lock();
			rect->setPosition(x, y);
			mtx.unlock();
		}
	};

class Line : public Drawable {
protected:
	sf::RectangleShape* line;
public:
	Line(sf::RectangleShape* rect) : Drawable(rect) {
		this->line = rect;
	}

	~Line() {
		delete line;
	}

	void transformFirstPoint(float x1, float y1) {
		mtx.lock();
		line->setPosition(x1, y1);
		mtx.unlock();
	}

	void transformSecondPoint(float x2, float y2) {
		mtx.lock();
		sf::Vector2f p1 = line->getPosition();
		float dX = x2 - p1.x;
		float dY = y2 - p1.y;

		float rot = atan2(dY, dX) * 57.2958f;
		line->setSize(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), line->getSize().y));
		line->setOrigin(0, line->getSize().y / 2);
		line->setRotation(rot);
		mtx.unlock();
	}

	void transform(float x1, float y1, float x2, float y2) {
		mtx.lock();
		float dX = x2 - x1;
		float dY = y2 - y1;

		float rot = atan2(dY, dX) * 57.2958f;
		line->setSize(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), line->getSize().y));
		line->setOrigin(0, line->getSize().y / 2);
		line->setRotation(rot);
		line->setPosition(x1, y1);
		mtx.unlock();
	}
};

class Circle : public Drawable {
	protected:
		sf::CircleShape* circle;
	public:
		Circle() = delete;
		Circle(sf::CircleShape* circle) : Drawable(circle) {
			this->circle = circle;
		}	

		~Circle() {
			delete circle;
		}

		void transform(float x, float y) {
			mtx.lock();
			circle->setPosition(x, y);
			mtx.unlock();
		}

		void setRadius(float radius) {
			mtx.lock();
			circle->setRadius(radius);
			mtx.unlock();
		}
	};

class Text : public Drawable {
	protected:
		sf::Text* text;
	public:
		//It does not make sense to construct a ThreadSafeShape without a shape.
		Text() = delete;
		/*  You have to handle these shapes in pointers, because mutices can't be moved properly => with pointers there is no need for a copy constructor,
		avoids mistakes*/
		Text(const Text& copy) = delete;
		//You have to handle these shapes in pointers, because mutices can't be moved properly => no move constructor
		Text(Text&& toMove) = delete;

		Text(sf::Text* text) : Drawable(text) {
			this->text = text;
		}

		~Text() {
			delete text;
		}

		void transform(float x, float y) {
			mtx.lock();
			text->setPosition(x, y);
			mtx.unlock();
		}
	};

}