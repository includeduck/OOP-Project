// GraphicsManager.h
#pragma once

#include <SFML/Graphics.hpp>

class GraphicsManager {
private:
    sf::RenderWindow window;
    sf::View fixedView;  // Maintain consistent viewport

public:
    GraphicsManager(unsigned int width, unsigned int height, const char* title)
        : window(sf::VideoMode(width, height), title,
            sf::Style::Titlebar | sf::Style::Close)  // Disable resizing
    {
        window.setFramerateLimit(60);
        fixedView = sf::View(sf::FloatRect(0, 0, width, height));
        window.setView(fixedView);
    }

    // Prevent window copying
    GraphicsManager(const GraphicsManager&) = delete;
    GraphicsManager& operator=(const GraphicsManager&) = delete;

    bool isOpen() const { return window.isOpen(); }
    bool pollEvent(sf::Event& event) { return window.pollEvent(event); }

    void clear(const sf::Color& color = sf::Color::Black) {
        window.clear(color);
    }

    void draw(const sf::Drawable& drawable) {
        window.draw(drawable);
    }

    void display() {
        window.display();
        // Maintain fixed view after display
        window.setView(fixedView);
    }

    sf::RenderWindow& getWindow() { return window; }

    // viewport control
    void setView(const sf::View& view) { window.setView(view); }
    const sf::View& getView() const { return window.getView(); }
    void resetView() { window.setView(fixedView); }

    // Get window dimensions
    sf::Vector2u getSize() const { return window.getSize(); }
};