#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"

class ThreadSafeDrawable {
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

class ThreadSafeShape : public ThreadSafeDrawable {
protected:
	sf::Shape* shape;
public:
	//It does not make sense to construct a ThreadSafeShape without a shape.
	ThreadSafeShape() = delete;
	/*  You have to handle these shapes in pointers, because mutices can't be moved properly => with pointers there is no need for a copy constructor,
	avoids mistakes*/
	ThreadSafeShape(const ThreadSafeShape& copy) = delete;
	//You have to handle these shapes in pointers, because mutices can't be moved properly => no move constructor
	ThreadSafeShape(ThreadSafeShape&& toMove) = delete;

	ThreadSafeShape(sf::Shape* shape) {
		this->shape = shape;
	}

	~ThreadSafeShape() {
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

class ThreadSafeText : public ThreadSafeDrawable {
protected:
	sf::Text* text;
public:
	//It does not make sense to construct a ThreadSafeShape without a shape.
	ThreadSafeText() = delete;
	/*  You have to handle these shapes in pointers, because mutices can't be moved properly => with pointers there is no need for a copy constructor,
	avoids mistakes*/
	ThreadSafeText(const ThreadSafeText& copy) = delete;
	//You have to handle these shapes in pointers, because mutices can't be moved properly => no move constructor
	ThreadSafeText(ThreadSafeText&& toMove) = delete;

	ThreadSafeText(sf::Text* text) {
		this->text = text;
	}

	~ThreadSafeText() {
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