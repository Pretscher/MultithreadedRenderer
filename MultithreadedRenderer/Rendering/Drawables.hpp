#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"
class ThreadSafeShape {
protected:
	sf::Shape* shape;
private:
	std::mutex mtx;
public:
	ThreadSafeShape() {}

	ThreadSafeShape(sf::Shape* shape) {
		this->shape = shape;
	}

	~ThreadSafeShape() {

	}

	ThreadSafeShape(const ThreadSafeShape& copy) {
		this->shape = copy.shape;
	}

	ThreadSafeShape(ThreadSafeShape&& toMove) {
		this->shape = toMove.shape;
	}

	void transform(int x, int y) {
		mtx.lock();
		shape->setPosition(x, y);
		mtx.unlock();
	}
	
	void drawOnce();

	void lock() {
		mtx.lock();
	}

	void unlock() {
		mtx.unlock();
	}

	/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced
	*/
	void draw();
};
