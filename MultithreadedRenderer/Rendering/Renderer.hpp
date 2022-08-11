#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "SFML/Graphics.hpp"

#include "ThreadSafeObjects.hpp"

struct TextureToLoad {
public:
    std::string path;
    bool repeat;
    sf::Shape* toApply;
};

class Renderer {
private:
	//Threadsafe objects that are cummulatively locked, drawn and individually unlocked each frame
    static std::vector<ts::Drawable> permanentObjects;

    static std::thread* renderingThread;
    static std::mutex isDrawing;
    static void threadInit();
    static void loop();

    //SFML always uses the dimensions of window creation, which means we only have to save these once in the constructor.
    static int xPixels, yPixels;
    static sf::RectangleShape* background;
public:
    static sf::RenderWindow* window;
    /** Creates a window and starts a seperate drawing thread.
    */
    static void initWindow() {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8.0;
        window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Rendering!", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize, settings);
        window->setActive(false);
        xPixels = window->getSize().x;
        yPixels = window->getSize().y;
        window->setFramerateLimit(60);

        renderingThread = new std::thread(&Renderer::threadInit);
    }
	
    static void addBackground(std::string texturePath, bool repeat) {
        background = new sf::RectangleShape(sf::Vector2f(xPixels, yPixels));
        queueTextureLoading(texturePath, repeat, background);
        
        /*We want a threadsafe shape here so that a potential future change to the background from the main thread is safe.*/
        isDrawing.lock();
        //insert at start of vector so that it is always in the background
        permanentObjects.insert(permanentObjects.begin(), ts::Rect(background));
        isDrawing.unlock();
    }

    //Drawing--------------------------------------------------------------------------------------------------------------------------------------
	
    static void addPermanentObject(ts::Drawable& object) {
        isDrawing.lock();//dont add objects while drawing. 
        permanentObjects.push_back(object);
        isDrawing.unlock();
    }
	
    static void deleteObject(const ts::Drawable& drawable) {
        isDrawing.lock();
        for (unsigned int i = 0; i < permanentObjects.size(); i++) {
            if (permanentObjects[i] == drawable) {
                permanentObjects[i].freeMemory();
                permanentObjects.erase(permanentObjects.begin() + i);
                isDrawing.unlock();
                return;
            }
        }
        //Object not in array, all good. just unlock.
        std::cout << "Warning: Object to delete not in array. This is probably a bug." << std::endl;
        isDrawing.unlock();
    }
	
    static ts::Drawable removeObject(const ts::Drawable& drawable) {
        ts::Drawable extracted;
        isDrawing.lock();
        for (unsigned int i = 0; i < permanentObjects.size(); i++) {
            if (permanentObjects[i] == drawable) {
                extracted = permanentObjects[i];
                permanentObjects.erase(permanentObjects.begin() + i);
                isDrawing.unlock();
                return extracted;
            }
        }
        //Object not in array, all good. just unlock.
        std::cout << "Warning: Object to delete not in array. This is probably a bug." << std::endl;
        isDrawing.unlock();
		return extracted;
    }
    static void drawFrame();

	//End of Rendering--------------------------------------------------------------------------------------------------------------------------------

    static void freeAllMemory() {
        delete window;
        delete background;
		permanentObjects.clear();
    }
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
    static std::vector<TextureToLoad> texturesToLoad;
    static std::mutex loadingMtx;

    /* Call before drawing! Loads all the textures in "texturesToLoad" by filling the empty Texture pointers in "loadedTextures" and removing the entry from "texturesToLoad"*/
    static void loadAllTextures();

public:
    /* Loads the texture in the Rendering Thread before the next drawing operation. Until it is loaded, returns a pointer to an empty texture.
    */
    static void queueTextureLoading(std::string path, bool repeat, sf::Shape* toApply) {
        if (loadedTextures.count(path) == 0) {
            loadingMtx.lock();
            texturesToLoad.push_back(TextureToLoad(path, repeat, toApply));
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