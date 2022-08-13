#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

//This cpp only exists, because ThreadSafeObjects.hpp and Renderer.hpp would include each other => We implement functions that use the Renderer here.

void ts::Drawable::initDrawableAfterConstruction(sf::Drawable* drawable) {
	this->drawable = drawable;
	Renderer::addPermanentObject(this);
}

ts::Drawable::~Drawable() {
	Renderer::removePermanentObject(this);
	delete drawable;
}

void ts::Drawable::draw() {
	Renderer::window->draw(*drawable);
}

void ts::Drawable::setPriority(int priority) {
	this->priority = priority;
	Renderer::updatePriority(this);
}

void ts::Shape::addTexture(std::string texturePath, bool repeat) {
	Renderer::queueTextureLoading(texturePath, repeat, shape);
}