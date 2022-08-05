#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"

namespace ts {

	class Drawable {
	protected:
		std::mutex mtx;
	public:
		void lock() {
			mtx.lock();
		}

		void unlock() {
			mtx.unlock();
		}

		void drawOnce();
		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced
		*/
		virtual void draw() = 0;
	};

	class Shape : public Drawable {
	protected:
		sf::Shape* shape;
	public:
		//It does not make sense to construct a ThreadSafeShape without a shape.
		Shape() = delete;
		/*  You have to handle these shapes in pointers, because mutices can't be moved properly => with pointers there is no need for a copy constructor,
		avoids mistakes*/
		Shape(const Shape& copy) = delete;
		//You have to handle these shapes in pointers, because mutices can't be moved properly => no move constructor
		Shape(Shape&& toMove) = delete;

		Shape(sf::Shape* shape) {
			this->shape = shape;
		}

		~Shape() {
			delete shape;
		}


		void transform(int x, int y) {
			mtx.lock();
			shape->setPosition(x, y);
			mtx.unlock();
		}

		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced
		*/
		void draw() override;

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

		Text(sf::Text* text) {
			this->text = text;
		}

		~Text() {
			delete text;
		}

		void transform(int x, int y) {
			mtx.lock();
			text->setPosition(x, y);
			mtx.unlock();
		}
		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced
		*/
		void draw() override;

	};

}