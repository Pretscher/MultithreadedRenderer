#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"


ts::Drawable::~Drawable() {
	mtx.lock();
	Renderer::removePermanentObject(this);
	mtx.unlock();
}

void ts::Drawable::draw() {
	Renderer::window->draw(*drawable);
}


