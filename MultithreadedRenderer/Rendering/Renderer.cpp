#pragma once
#include "Renderer.hpp"

sf::RenderWindow* Renderer::window;

std::vector<ts::Drawable*> Renderer::nextFrame;
std::vector<ts::Drawable*> Renderer::currentFrame;
std::vector<ts::Drawable*> Renderer::permanentObjects;
std::thread* Renderer::drawingThread;
std::mutex Renderer::isDrawing;

int Renderer::xPixels;
int Renderer::yPixels;

void Renderer::drawNextFrame() {
    //skip frames if frame count is faster than drawn frames
    while (getFramesToDraw() > getDrawnFrames()) {
        isDrawing.lock();
        nextFrame.clear();//skip next frame by clearing it's drawables
        isDrawing.unlock();
        incrementDrawnFrames();//simulate having drawn this frame
    }
    incrementFrameToDraw();
}

void Renderer::joinDrawingThread() {
    drawingThread->join();
    delete drawingThread;
}

//Protected Methods--------------------------------------------------------------------------------------------------------------------------------------
    

std::mutex Renderer::framesToDrawMutex;
long Renderer::framesToDraw;
    int Renderer::getFramesToDraw() {
        int temp;
        framesToDrawMutex.lock();
        temp = framesToDraw;
        framesToDrawMutex.unlock();
        return temp;
    }
    void Renderer::incrementFrameToDraw() {
        framesToDrawMutex.lock();
        framesToDraw++;
        framesToDrawMutex.unlock();
    }


    std::mutex Renderer::drawnFramesMutex;
    long Renderer::drawnFrames;
    void Renderer::incrementDrawnFrames() {
        drawnFramesMutex.lock();
        drawnFrames++;
        drawnFramesMutex.unlock();
    }
    int Renderer::getDrawnFrames() {
        int temp;
        drawnFramesMutex.lock();
        temp = drawnFrames;
        drawnFramesMutex.unlock();
        return temp;
    }

   
    void lockVector(std::vector<ts::Drawable*>& vector ) {
        for (int i = 0; i < vector.size(); i++) {
            vector[i]->lock();
        }
    }

    void Renderer::drawFrame() {
        window->clear();
        isDrawing.lock();
		//to stop a drawable from being drawn by the renderer, it is deleted externally. Thus we have to check for nullptrs and remove them from the vectors.
        lockVector(currentFrame);
        lockVector(permanentObjects);
		
        //draw temporary objects
        for (int i = 0; i < currentFrame.size(); i++) {
            currentFrame[i]->draw();
            currentFrame[i]->unlock();
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
            if (getDrawnFrames() < getFramesToDraw()) {
                drawFrame();
                incrementDrawnFrames();
            }
        }
    }

    //Private Methods--------------------------------------------------------------------------------------------------------------------------------------

    void Renderer::changeFrames() {
        isDrawing.lock();
        currentFrame = move(nextFrame);
        isDrawing.unlock();
    }


