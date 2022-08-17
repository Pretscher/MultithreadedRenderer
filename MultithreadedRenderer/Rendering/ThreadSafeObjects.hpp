#pragma once
#include <mutex>
#include "SFML/Graphics.hpp"
namespace ts {
	class Drawable {
	protected:
		std::mutex mtx;
		sf::Drawable* drawable = nullptr;
		std::mutex drawMeMtx;
		bool drawMe = true;
		void initDrawableAfterConstruction(sf::Drawable* drawable);
	public:
		Drawable() {}
		Drawable(const Drawable& rect) = delete;
		Drawable(Drawable&& rect) = delete;
		~Drawable();
		
		//Prevents all changes to the drawable (transformations, resizings, recolorings etc.) until the lock is released with unlock(). 
		void lock() {
			mtx.lock();
		}

		//When called, all changes to the drawable are allowed again.
		void unlock() {
			mtx.unlock();
		}

		void hide() {
			drawMeMtx.lock();
			drawMe = false;
			drawMeMtx.unlock();
		}

		void show() {
			drawMeMtx.lock();
			drawMe = true;
			drawMeMtx.unlock();
		}

		bool isShown() {
			drawMeMtx.lock();
			bool temp = drawMe;
			drawMeMtx.unlock();
			return temp;
		}

		virtual void applyChanges() {}
		/** ONLY CALL IN RENDERER! If you call it from anywhere else, it is not thread-synced*/
		virtual void draw();
	};

	class Shape : public Drawable {
	protected:
		sf::Shape* shape = nullptr;
		Shape() {};
		void initShapeAfterConstruction(sf::Shape* shape) {
			this->shape = shape;
			initDrawableAfterConstruction(shape);
		}
		/**
		 * @brief Add an outline for this shape.
		 *
		 * @param color
		 * @return Shape* this
		 */
	protected:
		float actualX = 0.0f, actualY = 0.0f, actualWidth = 0.0f, actualHeight = 0.0f;
		bool positionChanged = false, sizeChanged = false, colorChanged = false;
		sf::Color actualColor;
		std::mutex actualDataMtx;
	public:
		void applyChanges() override {
			actualDataMtx.lock();
			if (positionChanged == true) {
				shape->setPosition(actualX, actualY);
			}
			if (sizeChanged == true) {
				shape->setScale(sf::Vector2f(actualWidth, actualHeight));
			}
			if (colorChanged == true) {
				shape->setFillColor(actualColor);
			}
			actualDataMtx.unlock();
		}

		
		void transform(float x, float y) {
			actualDataMtx.lock();
			actualX = x;
			actualY = y;
			positionChanged = true;
			actualDataMtx.unlock();
		}

		void resize(float width, float height) {
			actualDataMtx.lock();
			actualWidth = width;
			actualHeight = height;
			sizeChanged = true;
			actualDataMtx.unlock();
		}

		void setColor(sf::Color color) {
			actualDataMtx.lock();
			actualColor = color;
			colorChanged = true;
			actualDataMtx.unlock();
		}

		void setX(float x) {
			transform(x, actualY);
		}

		void setY(float y) {
			transform(actualX, y);
		}
		
		int getX() {
			actualDataMtx.lock();
			int x = actualX;
			actualDataMtx.unlock();
			return x;
		}

		int getY() {
			actualDataMtx.lock();
			int y = actualY;
			actualDataMtx.unlock();
			return y;
		}

		/**
		 * @brief Load a texture and add it to this shape.
		 *
		 * @param texturePath
		 * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
		 * @return Shape* this
		 */
		void addTexture(std::string texturePath, bool repeat);

		void addOutline(sf::Color color, float thickness) {
			mtx.lock();
			shape->setOutlineColor(color);
			shape->setOutlineThickness(thickness);
			mtx.unlock();
		}
		
		sf::Color getColor() {
			actualDataMtx.lock();
			sf::Color temp = actualColor;
			actualDataMtx.unlock();
			return temp;
		}

		sf::Vector2f getPosition() {
			mtx.lock();
			sf::Vector2f temp(actualX, actualY);
			mtx.unlock();
			return temp;
		}
	};

	class Rect : public Shape {
	protected:
		sf::RectangleShape* rect;
	public:
		Rect() = delete;

		Rect(sf::RectangleShape* rect) : rect(rect) {
			this->rect = rect;
			initShapeAfterConstruction(rect);
		}
		
		Rect(float x, float y, float width, float height) : rect(new sf::RectangleShape(sf::Vector2f(width, height))) {
			rect->setPosition(x, y);
			initShapeAfterConstruction(rect);
		}


		//Builder functions generated from implementations in shape (done this way to avoid duplicate code)---------------------------------------------------

		Rect* addOutline(sf::Color color, float thickness) {
			Shape::addOutline(color, thickness);
			return this;
		}
		
		Rect* setColor(sf::Color color) {
			Shape::setColor(color);
			return this;
		}


		Rect* addTexture(std::string texturePath, bool repeat) {
			Shape::addTexture(texturePath, repeat);
			return this;
		}

		sf::Vector2f getSize() {
			mtx.lock();
			sf::Vector2f temp(actualWidth, actualHeight);
			mtx.unlock();
			return temp;
		}
	};

	class Line : public Drawable {
	protected:
		sf::RectangleShape* line;
		float x2, y2;
	public:
		Line() = delete;

		Line(float x1, float y1, float x2, float y2) 
			: line(new sf::RectangleShape(sf::Vector2f(0.0f, 5.0f))) {//init with a default thickness of 5 pixels
			this->x2 = x2;
			this->y2 = y2;
			transform(x1, y1, x2, y2);
			
			initDrawableAfterConstruction(this->line);
		}

		Line* setThickness(float thickness) {
			mtx.lock();
			line->setSize(sf::Vector2f(line->getSize().x, thickness));
			mtx.unlock();
			return this;
		}

		Line* transformFirstPoint(float x1, float y1) {
			transform(x1, y1, this->x2, this->y2);
			return this;
		}

		Line* transformSecondPoint(float x2, float y2) {
			transform(line->getPosition().x, line->getPosition().y, x2, y2);
			return this;
		}

		Line* transform(float x1, float y1, float x2, float y2) {
			mtx.lock();
			float dX = x2 - x1;
			float dY = y2 - y1;

			float rot = atan2(dY, dX) * 57.2958f;
			line->setSize(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), line->getSize().y));
			line->setOrigin(0, line->getSize().y / 2);
			line->setRotation(rot);
			line->setPosition(x1, y1);
			mtx.unlock();
			return this;
		}

		Line* setColor(sf::Color color) {
			mtx.lock();
			line->setFillColor(color);
			mtx.unlock();
			return this;
		}
	};

	class Circle : public Shape {
	protected:
		sf::CircleShape* circle;
	public:
		Circle() = delete;
		Circle(sf::CircleShape* circle) : circle(circle) {
			initShapeAfterConstruction(this->circle);
		}
		
		Circle(float x, float y, float radius) : circle(new sf::CircleShape(radius)) {
			circle->setPosition(x, y);
			initShapeAfterConstruction(this->circle);
		}

		Circle* addOutline(sf::Color color, float thickness) {
			Shape::addOutline(color, thickness);
			return this;
		}

		Circle* setColor(sf::Color color) {
			Shape::setColor(color);
			return this;
		}

		Circle* addTexture(std::string texturePath, bool repeat) {
			Shape::addTexture(texturePath, repeat);
			return this;
		}
	protected:
		int actualRadius = 0;
		bool radiusChanged = false;
	public:
		Circle* setRadius(float radius) {
			actualDataMtx.lock();
			actualRadius = radius;
			radiusChanged = true;
			actualDataMtx.unlock();
			return this;
		}

		float getRadius() {
			actualDataMtx.lock();
			float temp = actualRadius;
			actualDataMtx.unlock();
			return temp;
		}

		void applyChanges() {
			Shape::applyChanges();
			actualDataMtx.lock();
			if (radiusChanged == true) {
				circle->setRadius(actualRadius);
			}
			actualDataMtx.unlock();
		}
	};

	class Text : public Drawable {
	protected:
		sf::Text* text;
	public:
		Text() = delete;
		Text(float x, float y, std::string displayedText) : text(new sf::Text()) {
			text = new sf::Text();
			text->setPosition(x, y);
			text->setString(displayedText);
			this->setFont("Fonts/calibri.ttf");//default font is calibri
			initDrawableAfterConstruction(text);
		}

		Text(std::string displayedText) : text(new sf::Text()) {
			text = new sf::Text();
			text->setPosition(0.0f, 0.0f);
			text->setString(displayedText);
			this->setFont("Fonts/calibri.ttf");//default font is calibri
			initDrawableAfterConstruction(text);
		}

		Text* centerToRect(int x, int y, int width, int height) {
			std::string temp = text->getString().toAnsiString();
			bool bold = (text->getStyle() == sf::Text::Bold);
			auto font = text->getFont();
			int maxHeight = 0;
			for (int x = 0; x < text->getString().getSize(); x++)
			{
				sf::Uint32 Character = temp.at(x);
				const sf::Glyph& CurrentGlyph = font->getGlyph(Character, text->getCharacterSize(), bold);

				int Height = CurrentGlyph.bounds.height;

				if (maxHeight < Height) {
					maxHeight = Height;
				}
			}

			sf::FloatRect rect = text->getGlobalBounds();

			rect.left = ((float)width / 2) - (rect.width / 2);
			rect.top = ((float)height / 2) - ((float)maxHeight / 2) - (rect.height - maxHeight) + (((float)rect.height - (text->getCharacterSize() * 1.5))) / 2;

			transform(rect.left + x, rect.top + y);
			return this;
		}

		Text* centerToRect(ts::Rect* rect) {
			return centerToRect(rect->getPosition().x, rect->getPosition().y, rect->getSize().x, rect->getSize().y);//returns this
		}

		Text* setFont(std::string fontPath) {
			sf::Font* font = loadFont(fontPath);
			setFont(font);
			return this;
		}

		Text* setFont(sf::Font* font) {
			mtx.lock();
			text->setFont(*font);
			mtx.unlock();
			return this;
		}


		Text* setColor(sf::Color color) {
			mtx.lock();
			text->setFillColor(color);
			mtx.unlock();
			return this;
		}

		Text* setString(std::string string) {
			mtx.lock();
			text->setString(string);
			mtx.unlock();
			return this;
		}

		Text* setCharacterSize(float characterSize) {
			mtx.lock();
			text->setCharacterSize(characterSize);
			mtx.unlock();
			return this;
		}

		Text* transform(float x, float y) {
			mtx.lock();
			text->setPosition(x, y);
			mtx.unlock();
		}

	private:
		//loaded in main thread because loading a font is not incredibly costly and I can't be bothered to put it into the Rendering thread like texture loading
		sf::Font* loadFont(std::string fontPath);
	};
}