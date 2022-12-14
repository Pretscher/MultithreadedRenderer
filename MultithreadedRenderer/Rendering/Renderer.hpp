#pragma once
#include "ThreadSafeObjects.hpp"
#include "SleepAPI.hpp"
#include "Mouse.hpp"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "SFML/Graphics.hpp"


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
	static std::mutex permanentObjectMtx;
	static std::vector<ts::Drawable*> changedObjects;
	static std::mutex changedObjectMtx;
	static std::mutex drawingMtx;
	static std::thread* renderingThread;

	static void threadInit();
	static void loop();

	//SFML always uses the dimensions of window creation, which means we only have to save these once in the constructor.
	static int xPixels, yPixels;
public:
	static sf::RenderWindow* window;
	/** Creates a window and starts a seperate drawing thread.
	*/

	static void stopDrawing() {
		drawingMtx.lock();
	}

	static void continueDrawing() {
		drawingMtx.unlock();
	}

	static void initSettings() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Rendering!", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize, settings);
		window->setActive(false);
		xPixels = window->getSize().x;
		yPixels = window->getSize().y;
		window->setFramerateLimit(60);
	}
public:

	static void init() {
		initSettings();
	}

	static void startEventloop(void (*callbackEventloop)()) {
		renderingThread = new std::thread(&Renderer::threadInit);

		SleepAPI sleepAPI{};//for way more accurate sleeps than this_thread::sleep allows
		sf::Event eventCatcher{};
		//Event loop of main thread main thread
		while (window->isOpen()) {
			sleepAPI.millisleep(16);
			while (Renderer::window->pollEvent(eventCatcher)) {
				if (eventCatcher.type == sf::Event::Closed) {
					Renderer::window->close();
					break;
				}
			}
			callbackEventloop();
			Mouse::update();
		}
		Renderer::joinDrawingThread();//when finished, join the drawing thread before exiting
	}

	static void addBackground(std::string texturePath, bool repeat) {
		ts::Rect* background = (new ts::Rect(0, 0, (float)xPixels, (float)yPixels))->addTexture(texturePath, repeat);//0 is lowest priority => drawn in the back.
		removePermanentObject(background);//it was added at the end of the line, but we want it to be drawn first.
		permanentObjects.insert(permanentObjects.begin(), background);//reinsert at front
	}

	//Drawing--------------------------------------------------------------------------------------------------------------------------------------

	static void addPermanentObject(ts::Drawable* object) {
		permanentObjectMtx.lock();//dont add objects while drawing. 
		permanentObjects.push_back(object);
		permanentObjectMtx.unlock();
	}

	static void addAsChangedObject(ts::Drawable* object) {
		changedObjectMtx.lock();
		changedObjects.push_back(object);
		changedObjectMtx.unlock();
	}

	/*Call this in the destructor of an Object and it will remove itself from the drawing array when deleted.*/

	static void removePermanentObject(ts::Drawable* object) {
		permanentObjectMtx.lock();

		for (int i = permanentObjects.size() - 1; i >= 0; i--) {//search from the back because those objects are more probable to be temporary
			if (permanentObjects[i] == object) {
				auto a = permanentObjects[permanentObjects.size() - 1];
				auto b = permanentObjects[permanentObjects.size() - 2];
				auto c = permanentObjects[permanentObjects.size() - 3];
				auto d = permanentObjects[permanentObjects.size() - 4];
				permanentObjects.erase(permanentObjects.begin() + i);

				auto e = permanentObjects[permanentObjects.size() - 1];
				auto f = permanentObjects[permanentObjects.size() - 2];
				auto g = permanentObjects[permanentObjects.size() - 3];
				auto h = permanentObjects[permanentObjects.size() - 4];
				break;
			}
		}
		permanentObjectMtx.unlock();
		//can also be in changed objects, remove it from there as well.
		changedObjectMtx.lock();
		for (int i = changedObjects.size() - 1; i >= 0; i--) {
			if (changedObjects[i] == object) {
				changedObjects.erase(changedObjects.begin() + i);
				break;
			}
		}
		changedObjectMtx.unlock();
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
	/** @brief !Starting directory is Rendering/recources!
	* Loads the texture in the Rendering Thread before the next drawing operation. Until it is loaded, returns a pointer to an empty texture.*/
	static void queueTextureLoading(std::string path, bool repeat, sf::Shape* toApply) {
		std::string fullPath = "Rendering/recources/" + path;
		if (loadedTextures.count(path) == 0) {
			loadingMtx.lock();
			texturesToLoad.push_back(TexturedObjectToLoad(fullPath, repeat, toApply));
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