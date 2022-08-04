#include "Drawables.hpp"
#include "Renderer.hpp"

void ThreadSafeShape::draw() {
	Renderer::window->draw(*shape);
}

void ThreadSafeShape::drawOnce() {
	Renderer::drawOnce(this);
}

