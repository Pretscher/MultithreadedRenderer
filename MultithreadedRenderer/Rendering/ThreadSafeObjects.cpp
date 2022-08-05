#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"


ts::Drawable::~Drawable() {
	mtx.lock();
	Renderer::removePermanentObject(this);
	mtx.unlock();
}

void ts::Shape::draw() {
	Renderer::window->draw(*shape);
}

void ts::Text::draw() {
	Renderer::window->draw(*text);
}


