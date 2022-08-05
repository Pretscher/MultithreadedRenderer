#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

void ThreadSafeDrawable::drawOnce() {
	Renderer::drawOnce(this);
}

void ThreadSafeShape::draw() {
	Renderer::window->draw(*shape);
}

void ThreadSafeText::draw() {
	Renderer::window->draw(*text);
}


