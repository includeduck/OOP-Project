// UIScreen.h
#pragma once

#include <SFML/Graphics.hpp>
#include "GraphicsManager.h"

// Abstract base for all game UI screens
class UIScreen
{
public:
    virtual ~UIScreen() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(double dt) = 0;
    virtual void draw(GraphicsManager& graphics) = 0;
	virtual bool isFinished() const = 0;
};