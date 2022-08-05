#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"

namespace ts {
	class Drawable {
	protected:
		std::mutex mtx;
	public:
		~Drawable();//implemented in the cpp file because it has to inform the renderer of the deletion, and the renderer includes this header.
		void lock() {
			mtx.lock();
		}

		void unlock() {
			if (mtx.try_lock() == false) {
				mtx.unlock();
			}
		}

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

		void transform(float x, float y) {
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

		void transform(float x, float y) {
			mtx.lock();
			text->setPosition(x, y);
			mtx.unlock();
		}
		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced
		*/
		void draw() override;

	};

}