#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

inline void ensurePlaceholderTexture(sf::Texture& tex)
{
    sf::Image img;
    img.create(2, 2, sf::Color::Magenta);
    tex.loadFromImage(img);
}

inline bool loadTextureOrPlaceholder(sf::Texture& tex, const char* path)
{
    if (path && tex.loadFromFile(path)) return true;
    std::cerr << "[Asset] Failed to load texture: " << (path ? path : "(null)") << "\n";
    ensurePlaceholderTexture(tex);
    return false;
}

inline bool loadFont(sf::Font& font, const char* path)
{
    if (path && font.loadFromFile(path)) return true;
    std::cerr << "[Asset] Failed to load font: " << (path ? path : "(null)") << "\n";
    return false;
}

inline bool openMusic(sf::Music& music, const char* path)
{
    if (path && music.openFromFile(path)) return true;
    std::cerr << "[Asset] Failed to open music: " << (path ? path : "(null)") << "\n";
    return false;
}

