#pragma once
#include "Renderer.hpp"

//Static defines------------------------------------------------------------------------------------------------------------------------------

sf::RenderWindow* Renderer::window;
int Renderer::xPixels;
int Renderer::yPixels;

//Multithreading------------------------------------------------------------------------------------------------------------------------------
std::thread* Renderer::renderingThread;
void Renderer::joinDrawingThread() {
	renderingThread->join();
	delete renderingThread;
}

void Renderer::threadInit() {
	window->setActive(true);
	loop();
}

void Renderer::loop() {
	while (window->isOpen()) {
		drawFrame();
	}
}

//Drawing--------------------------------------------------------------------------------------------------------------------------------------

std::vector<ts::Drawable*> Renderer::permanentObjects;
std::mutex Renderer::permanentObjectMtx;
std::vector<ts::Drawable*> Renderer::changedObjects;
std::mutex Renderer::changedObjectMtx;
void Renderer::drawFrame() {
	loadAllTextures();
	window->clear();
	permanentObjectMtx.lock();

	//lock all drawables before drawing, so that they are at the transformation state of the same frame.
	for (unsigned int i = 0; i < permanentObjects.size(); i++) {
		permanentObjects[i]->lock();
	}

	//draw permanent objects
	for (unsigned int i = 0; i < permanentObjects.size(); i++) {
		if (permanentObjects[i]->isShown() == true) {
			permanentObjects[i]->draw();
		}
		permanentObjects[i]->unlock();
	}
	permanentObjectMtx.unlock();
	window->display();

	changedObjectMtx.lock();
	for (unsigned int i = 0; i < changedObjects.size(); i++) {
		changedObjects[i]->applyChanges();
	}
	changedObjects.clear();
	changedObjectMtx.unlock();
}

//Textures-------------------------------------------------------------------------------------------------------------

std::map<std::string, sf::Texture*> Renderer::loadedTextures;
std::vector<TexturedObjectToLoad> Renderer::texturesToLoad;
std::mutex Renderer::loadingMtx;
void Renderer::loadAllTextures() {
	loadingMtx.lock();
	for (size_t i = 0; i < texturesToLoad.size(); i++) {
		TexturedObjectToLoad& toLoad = texturesToLoad[i];
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(toLoad.path)) {
			std::cout << "failed to load texture of path '" << toLoad.path << "'";
			loadingMtx.unlock();
			return;
		}
		if (toLoad.repeat == true) {
			sf::Image img = texture->copyToImage();
			if (texture->loadFromImage(img) == false) {
				std::cout << "failed to load texture image of path '" << toLoad.path << "'";
				loadingMtx.unlock();
				return;
			}
			texture->setRepeated(true);
		}
		//TODO: We dont actually need mutices here to tell the main thread that the texture changed, because the texture is only used in this thread. Add them anyway for safety.
		loadedTextures[toLoad.path] = texture;//was already added to map so that the pointer could be returned
		toLoad.setTexture(loadedTextures[toLoad.path]);
		texturesToLoad.erase(texturesToLoad.begin() + i);
	}
	loadingMtx.unlock();
}