#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

void ts::Drawable::drawOnce() {
	Renderer::drawOnce(this);
}

void ts::Shape::draw() {
	Renderer::window->draw(*shape);
}

void ts::Text::draw() {
	Renderer::window->draw(*text);
}


