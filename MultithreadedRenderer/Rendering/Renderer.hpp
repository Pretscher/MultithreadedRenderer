#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "SFML/Graphics.hpp"

#include "ThreadSafeObjects.hpp"

struct TexturedObjectToLoad {
public:
	TexturedObjectToLoad(std::string path, bool repeat, sf::Shape* toApply) : path(path), repeat(repeat), toApply(toApply) {}
	std::string path;
	bool repeat;
	sf::Shape* toApply;
	
	void setTexture(sf::Texture* texture) {
		toApply->setTexture(texture);
	}

	void setPriority(int priority) {
		this->priority = priority;
	}
	int getPriority() {
		return priority;
	}
	void makeBackground() {
		priority = 0;
	}
private:
	int priority = 1;//0 := lowest priority (backgrounds etc)
};

class Renderer {
private:
	//Threadsafe objects that are cummulatively locked, drawn and individually unlocked each frame
	static std::vector<ts::Drawable*> permanentObjects;

	static std::thread* renderingThread;
	static std::mutex isDrawing;
	static void threadInit();
	static void loop();

	//SFML always uses the dimensions of window creation, which means we only have to save these once in the constructor.
	static int xPixels, yPixels;
public:
	static sf::RenderWindow* window;
	/** Creates a window and starts a seperate drawing thread.
	*/
	static void init() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Rendering!", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize, settings);
		window->setActive(false);
		xPixels = window->getSize().x;
		yPixels = window->getSize().y;
		window->setFramerateLimit(60);

		renderingThread = new std::thread(&Renderer::threadInit);
	}

	static void addBackground(std::string texturePath, bool repeat) {
		(new ts::Rect(0, 0, (float)xPixels, (float)yPixels))->addTexture(texturePath, repeat)->setPriority(0);//0 is lowest priority => drawn in the back.
	}

	//Drawing--------------------------------------------------------------------------------------------------------------------------------------

	static void addPermanentObject(ts::Drawable* object) {
		isDrawing.lock();//dont add objects while drawing. 
		//insert at LAST place in array (drawing order) with right priority (=> drawn in front of its own priority group)
		for (int i = 0; i < permanentObjects.size(); i++) {
			if (permanentObjects[i]->getPriority() > object->getPriority()) {
				permanentObjects.insert(permanentObjects.begin() + i, object);
				isDrawing.unlock();
				return;
			}
		}
		//nothing with bigger priority in array => append
		permanentObjects.push_back(object);
		isDrawing.unlock();
	}


	static void updatePriority(ts::Drawable* object) {
		removePermanentObject(object);//erase in old place
		addPermanentObject(object);//reinsert at new place
	}
	
	/*Call this in the destructor of an Object and it will remove itself from the drawing array when deleted.*/

	static void removePermanentObject(ts::Drawable* object) {
		isDrawing.lock();
		for (int i = 0; i < permanentObjects.size(); i++) {
			if (permanentObjects[i] == object) {
				permanentObjects.erase(permanentObjects.begin() + i);
				isDrawing.unlock();
				return;
			}
		}
		isDrawing.unlock();
	}

	static void drawFrame();
	static void joinDrawingThread();

	//Utility-----------------------------------------------------------------------------------------------------------------------------------------

	//Width of the window panel, can't be asked directly from the window
	static int getPixelCountX() {
		return xPixels;
	}
	//Height of the window panel, can't be asked directly from the window
	static int getPixelCountY() {
		return yPixels;
	}

	//Texture loading---------------------------------------------------------------------------------------------------------------------------------
	//(This has to be done in Rendering thread because the window has to be activated. Also, doing this here relieves a lot of load from the main thread)
private:
	static std::map<std::string, sf::Texture*> loadedTextures;
	static std::vector<TexturedObjectToLoad> texturesToLoad;
	static std::mutex loadingMtx;

	/* Call before drawing! Loads all the textures in "texturesToLoad" by filling the empty Texture pointers in "loadedTextures" and removing the entry from "texturesToLoad"*/
	static void loadAllTextures();

public:
	/* Loads the texture in the Rendering Thread before the next drawing operation. Until it is loaded, returns a pointer to an empty texture.*/
	static void queueTextureLoading(std::string path, bool repeat, sf::Shape* toApply) {
		if (loadedTextures.count(path) == 0) {
			loadingMtx.lock();
			texturesToLoad.push_back(TexturedObjectToLoad(path, repeat, toApply));
			loadingMtx.unlock();
		}
	}

	bool isTextureLoaded(std::string path) {
		loadingMtx.lock();
		bool out = loadedTextures.count(path) > 0;
		loadingMtx.unlock();
		return out;
	}

	//only call when "isTextureLoaded" is true!
	sf::Texture* getLoadedTexture(std::string path) {
		return loadedTextures[path];
	}
};