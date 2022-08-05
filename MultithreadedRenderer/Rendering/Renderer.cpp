#pragma once
#include "Renderer.hpp"

sf::RenderWindow* Renderer::window;

std::vector<ts::Drawable*> Renderer::permanentObjects;
std::thread* Renderer::drawingThread;
std::mutex Renderer::isDrawing;

int Renderer::xPixels;
int Renderer::yPixels;


void Renderer::joinDrawingThread() {
    drawingThread->join();
    delete drawingThread;
}

//Protected Methods--------------------------------------------------------------------------------------------------------------------------------------

    void Renderer::drawFrame() {
        window->clear();
        isDrawing.lock();

		//lock all drawables before drawing, so that they are at the transformation state of the same frame.
        for (int i = 0; i < permanentObjects.size(); i++) {
            permanentObjects[i]->lock();
        }
		
        //draw permanent objects
        for (int i = 0; i < permanentObjects.size(); i++) {
            permanentObjects[i]->draw();
            permanentObjects[i]->unlock();
        }
        isDrawing.unlock();
        window->display();
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


