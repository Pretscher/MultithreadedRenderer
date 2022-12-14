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

		//used so that we dont add the same drawable multiple times to the Renderer. reset in Drawable::applyChanges, so call this when overriding!
		bool addedAsChanged = false;
		void prepareApplyingChanges();

	public:
		Drawable() {}
		Drawable(const Drawable& rect) = delete;
		Drawable(Drawable&& rect) = delete;
		virtual ~Drawable();

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

		bool isEqualTo(ts::Drawable& drawable) {
			return this->drawable == drawable.accessDrawable();
		}

		//There is no reason to call this externally.
		sf::Drawable* accessDrawable() {
			return drawable;
		}

		/*Call this from the renderer to apply commonand costly changes in the Rendering thread(prevents blocking of drawing)
		Override this in derived classes and add the actual functionality there. */
		virtual void applyChanges() {
			addedAsChanged = false;
		}
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
		bool positionChanged = false, colorChanged = false;

		sf::Color actualColor;
		std::mutex actualDataMtx;

	public:
		void applyChanges() override {
			actualDataMtx.lock();

			if (positionChanged == true) {
				shape->setPosition(actualX, actualY);
				positionChanged = false;
			}
			if (colorChanged == true) {
				shape->setFillColor(actualColor);
				colorChanged = false;
			}
			Drawable::applyChanges();

			actualDataMtx.unlock();
		}


		void transform(float x, float y) {
			actualDataMtx.lock();
			actualX = x;
			actualY = y;
			positionChanged = true;
			prepareApplyingChanges();
			actualDataMtx.unlock();
		}

		void setColor(sf::Color color) {
			actualDataMtx.lock();
			actualColor = color;
			colorChanged = true;
			prepareApplyingChanges();
			actualDataMtx.unlock();
		}

		void setX(float x) {
			transform(x, actualY);
		}

		void setY(float y) {
			transform(actualX, y);
		}

		float getX() {
			actualDataMtx.lock();
			float x = actualX;
			actualDataMtx.unlock();
			return x;
		}

		float getY() {
			actualDataMtx.lock();
			float y = actualY;
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

		Rect(float x, float y, float width, float height) : rect(new sf::RectangleShape(sf::Vector2f(width, height))) {
			rect->setPosition(x, y);
			actualX = x; actualY = y; actualWidth = width; actualHeight = height;
			initShapeAfterConstruction(rect);
		}

		~Rect() override {
			Drawable::~Drawable();
			delete rect;
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

		void applyChanges() {
			Shape::applyChanges();
			actualDataMtx.lock();
			if (sizeChanged == true) {
				rect->setSize(sf::Vector2f(actualWidth, actualHeight));
				sizeChanged = false;
			}
			actualDataMtx.unlock();
		}

		void resize(float width, float height) {
			actualDataMtx.lock();
			actualWidth = width;
			actualHeight = height;
			sizeChanged = true;
			prepareApplyingChanges();
			actualDataMtx.unlock();
		}

	protected:
		bool sizeChanged = false;
	};

	class Line : public Drawable {
	protected:
		sf::RectangleShape* line;
		float x2, y2;
	public:
		Line() = delete;

		~Line() override {
			Drawable::~Drawable();
			delete line;
		}

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
			actualX = x; actualY = y; actualRadius = radius;
			initShapeAfterConstruction(this->circle);
		}

		~Circle() override {
			Drawable::~Drawable();
			delete circle;
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
		float actualRadius = 0;
		bool radiusChanged = false;
	public:
		Circle* setRadius(float radius) {
			actualDataMtx.lock();
			actualRadius = radius;
			radiusChanged = true;
			prepareApplyingChanges();
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

		~Text() override {
			Drawable::~Drawable();
			delete text;
		}

		Text* centerToRect(float x, float y, float width, float height) {
			std::string temp = text->getString().toAnsiString();
			bool bold = (text->getStyle() == sf::Text::Bold);
			auto font = text->getFont();
			float maxHeight = 0;
			for (int x = 0; x < text->getString().getSize(); x++)
			{
				sf::Uint32 Character = temp.at(x);
				const sf::Glyph& CurrentGlyph = font->getGlyph(Character, text->getCharacterSize(), bold);

				float Height = CurrentGlyph.bounds.height;

				if (maxHeight < Height) {
					maxHeight = Height;
				}
			}

			sf::FloatRect rect = text->getGlobalBounds();

			rect.left = (width / 2) - (rect.width / 2);
			rect.top = (height / 2) - (maxHeight / 2) - (rect.height - maxHeight) + ((rect.height - ((float)text->getCharacterSize() * 1.5f))) / 2;

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

		Text* setCharacterSize(unsigned int characterSize) {
			mtx.lock();
			text->setCharacterSize(characterSize);
			mtx.unlock();
			return this;
		}

		Text* transform(float x, float y) {
			mtx.lock();
			text->setPosition(x, y);
			mtx.unlock();
			return this;
		}

	private:
		//loaded in main thread because loading a font is not incredibly costly and I can't be bothered to put it into the Rendering thread like texture loading
		sf::Font* loadFont(std::string fontPath);
	};
}