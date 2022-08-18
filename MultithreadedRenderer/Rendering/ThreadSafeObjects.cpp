#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

//This cpp only exists, because ThreadSafeObjects.hpp and Renderer.hpp would include each other => We implement functions that use the Renderer here.

void ts::Drawable::initDrawableAfterConstruction(sf::Drawable* drawable) {
    this->drawable = drawable;
    Renderer::addPermanentObject(this);
}

ts::Drawable::~Drawable() {
    Renderer::removePermanentObject(this);
}

void ts::Drawable::draw() {
    Renderer::window->draw(*drawable);
}

void ts::Drawable::prepareApplyingChanges() {
    //if multiple things were changed this frame, only add to renderer once
    if (addedAsChanged == false) {
        Renderer::addAsChangedObject(this);
        addedAsChanged = true;
    }
}

void ts::Shape::addTexture(std::string texturePath, bool repeat) {
    Renderer::queueTextureLoading(texturePath, repeat, shape);
}

static std::map<std::string, sf::Font*> loadedFonts;//we don't need to delete the fonts because this static map will persist until the program ends.
sf::Font* ts::Text::loadFont(std::string fontPath) {
    fontPath = "Rendering/recources/" + fontPath;
    if (loadedFonts.count(fontPath) == 0) {//if font not already in set, load it
        sf::Font* font = new sf::Font();
        if (!font->loadFromFile(fontPath)) {
            std::cout << "ERROR loading font with path '" << fontPath << "' coult not be loaded. Text will not be displayed. \n";
            //DO NOT select a font if there was an error loading, will cause in crash when trying to draw the text
            return {};
        }
        else {
            loadedFonts[fontPath] = font;
            return loadedFonts[fontPath];//retrieve from map so that the reference persists
        }
    }
    else {
        return loadedFonts[fontPath];
    }
}