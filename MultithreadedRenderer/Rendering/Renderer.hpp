#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "SFML/Graphics.hpp"

#include "ThreadSafeObjects.hpp"
class Renderer {
private:
    static std::vector<ts::Drawable*> permanentObjects;

    static std::thread* drawingThread;
    static std::mutex isDrawing;
public:
    static sf::RenderWindow* window;
    /** Creates a window and starts a seperate drawing thread.
    */
    static void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Rendering!", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
        window->setActive(false);
        xPixels = window->getSize().x;
        yPixels = window->getSize().y;
        window->setFramerateLimit(60);

        drawingThread = new std::thread(&Renderer::threadInit);
    }

    static void drawFrame();

    static void freeAllMemory() {
        delete window;
		
        for (unsigned int i = 0; i < permanentObjects.size(); i++) {
			delete permanentObjects[i];
        }
		permanentObjects.clear();
    }
	
    static void removePermanentObject(ts::Drawable* drawable) {
        isDrawing.lock();
        for (unsigned int i = 0; i < permanentObjects.size(); i++) {
            if (permanentObjects[i] == drawable) {
				permanentObjects.erase(permanentObjects.begin() + i);
                isDrawing.unlock();
				return;
			}
        }
        isDrawing.unlock();
    }

private:

    //SFML always uses the dimensions of window creation, which means we only have to save these once in the constructor.
    static int xPixels;
    static int yPixels;
public:
    //SFML always uses the dimensions of window creation, thus this is the amount of pixels your drawing space has on the x axis
    static int getPixelCountX() {
        return xPixels;
    }
    //SFML always uses the dimensions of window creation, thus this is the amount of pixels your drawing space has on the y axis
    static int getPixelCountY() {
        return yPixels;
    }

    static void addPermanentObject(ts::Drawable* object) {
        permanentObjects.push_back(object);
    }
    static void joinDrawingThread();

protected:
    static void threadInit();

    static void loop();
};