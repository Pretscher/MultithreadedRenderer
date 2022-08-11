#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

//This cpp only exists, because ThreadSafeObjects.hpp and Renderer.hpp would include each other => We implement functions that use the Renderer here.

void ts::Drawable::initDrawableAfterConstruction(sf::Drawable* drawable) {
	this->drawable = drawable;
	Renderer::addPermanentObject(*this);
}

void ts::Drawable::draw() {
	Renderer::window->draw(*drawable);
}

ts::Drawable::Drawable(Drawable&& rect) {
	mtx = rect.mtx;
	drawable = rect.drawable;
	rect.mtx = nullptr;
	rect.drawable = nullptr;
}

void ts::Shape::addTexture(std::string texturePath, bool repeat) {
	Renderer::queueTextureLoading(texturePath, repeat, shape);
}