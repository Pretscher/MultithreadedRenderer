#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "SFML/Graphics.hpp"

#include "ThreadSafeObjects.hpp"
class Renderer {
private:
    //save all drawables in this vector and draw them all at once, when update() is called.
    static std::vector<ts::Drawable*> nextFrame;
    static std::vector<ts::Drawable*> currentFrame;
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

    static void freeAllMemory() {
        delete window;
		
        for (int i = 0; i < permanentObjects.size(); i++) {
			delete permanentObjects[i];
        }
		permanentObjects.clear();
		
        for (int i = 0; i < nextFrame.size(); i++) {
            delete nextFrame[i];
        }
		nextFrame.clear();
    }
	
    static void removePermanentObject(ts::Drawable* drawable) {
        isDrawing.lock();
        for (int i = 0; i < permanentObjects.size(); i++) {
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

    static void drawOnce(ts::Drawable* toDraw) {
        isDrawing.lock();
        nextFrame.push_back(toDraw);
        isDrawing.unlock();
    }

    static void addPermanentObject(ts::Drawable* object) {
        permanentObjects.push_back(object);
    }
    /**
     * @brief Threadsafe way to tell this thread to draw all the objects pushed to it in the last frame.
     * Includes frame skipping.
     */
    static void drawNextFrame();
    static void joinDrawingThread();

protected:
    /**
     * @brief If next frame does not have the same counter as the current frame, then draw the next frame. Else wait for the next frame.
     *  If the frame counter increases too fast (rendering falls behind) this allows skipping frames 
     */
    static std::mutex framesToDrawMutex;
    static long framesToDraw;
    static int getFramesToDraw();
    static void incrementFrameToDraw();

    static std::mutex drawnFramesMutex;
    static long drawnFrames;
    static void incrementDrawnFrames();
    static int getDrawnFrames();

    static void drawFrame();

    static void threadInit();

    static void loop();

private:
    static void changeFrames();
};