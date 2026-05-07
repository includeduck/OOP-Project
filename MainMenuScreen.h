// MainMenuScreen.h
#pragma once

#include <SFML/Graphics.hpp>
#include "UIScreen.h"
#include "GraphicsManager.h"
#include "MainMenu.h"

static const float BUTTON_W = 400.0f;    // Increased width
static const float BUTTON_H = 80.0f;     // Increased height
static const float BUTTON_SPACING = 25.0f;
static const float HOVER_SCALE = 1.1f;
static const float SCALE_SPEED = 12.0f;
static const float BUTTON_RADIUS = 10.0f;

class MainMenuScreen : public UIScreen {
private:
    GraphicsManager* graphics;
    MainMenu* menuLogic;
    sf::Font* font;
    sf::Texture backgroundTex;
    sf::Sprite background;

    struct MenuItem
    {
        sf::RectangleShape btn;
        sf::Text lbl;
        float currScale;
        float targetScale;
        sf::Color baseColor;
        sf::Color hoverColor;
    } items[NUM_MENU_ITEMS];

    bool finished;
    int confirmedIndex;

public:
    MainMenuScreen(GraphicsManager* gfx, MainMenu* logic)
        : graphics(gfx), menuLogic(logic), finished(false), confirmedIndex(-1)
    {
        initializeResources();
        setupMenuItems();
    }

    ~MainMenuScreen() override {
        delete font;
    }

    void handleEvent(const sf::Event& e) override {
        if (e.type == sf::Event::MouseMoved) {
            handleMouseMove(sf::Vector2f(e.mouseMove.x, e.mouseMove.y));
        }
        else if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            handleMouseClick(sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
        }
        else if (e.type == sf::Event::KeyPressed) {
            handleKeyPress(e.key.code);
        }
    }

    void update(double dt) override {
        updateAnimations(dt);
    }

    void draw(GraphicsManager& g) override {
        g.clear();
        g.draw(background);
        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            g.draw(items[i].btn);
            g.draw(items[i].lbl);
        }
        g.display();
    }

    bool isFinished() const override { return finished; }
    int getSelection() const { return confirmedIndex; }

private:
    void initializeResources() {
        font = new sf::Font();
        font->loadFromFile("assets/main_font.ttf");

        backgroundTex.loadFromFile("assets/menu_bg.png");
        background.setTexture(backgroundTex);
        background.setScale(
            graphics->getSize().x / backgroundTex.getSize().x,
            graphics->getSize().y / backgroundTex.getSize().y
        );
    }
    
    void setupMenuItems() {
        auto winSize = graphics->getSize();
        float totalH = NUM_MENU_ITEMS * BUTTON_H + (NUM_MENU_ITEMS - 1) * BUTTON_SPACING;
        float startY = (winSize.y - totalH) / 2.0f;
        float centerX = winSize.x / 2.0f;

        const sf::Color baseColor(70, 70, 200, 200);
        const sf::Color hoverColor(100, 200, 100, 220);

        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            MenuItem& mi = items[i];
            // Button setup
            mi.btn.setSize({ BUTTON_W, BUTTON_H });
            mi.btn.setOrigin(BUTTON_W / 2, BUTTON_H / 2);
            mi.btn.setPosition(centerX, startY + i * (BUTTON_H + BUTTON_SPACING) + BUTTON_H / 2);
            mi.btn.setFillColor(baseColor);
            mi.btn.setOutlineThickness(3);
            mi.btn.setOutlineColor(sf::Color(255, 255, 255, 150));

            // Label setup
            mi.lbl.setFont(*font);
            mi.lbl.setCharacterSize(32);  // Increased font size
            mi.lbl.setString(menuLogic->getItemName(i));
            mi.lbl.setFillColor(sf::Color::White);
            centerText(mi.lbl, mi.btn.getPosition());

            // Animation state
            mi.currScale = 1.0f;
            mi.targetScale = 1.0f;
            mi.baseColor = baseColor;
            mi.hoverColor = hoverColor;
        }
    }

    void handleMouseMove(const sf::Vector2f& pos) {
        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            MenuItem& mi = items[i];
            bool hovered = mi.btn.getGlobalBounds().contains(pos);
            mi.targetScale = hovered ? HOVER_SCALE : 1.0f;
            mi.btn.setFillColor(hovered ? mi.hoverColor : mi.baseColor);
            if (hovered) menuLogic->selectItem(i);
        }
    }

    void handleMouseClick(const sf::Vector2f& pos) {
        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            if (items[i].btn.getGlobalBounds().contains(pos)) {
                confirmSelection(i);
            }
        }
    }

    void handleKeyPress(sf::Keyboard::Key code) {
        int idx = menuLogic->getHighlightedIndex();
        if (code == sf::Keyboard::Up) {
            idx = (idx + NUM_MENU_ITEMS - 1) % NUM_MENU_ITEMS;
        }
        else if (code == sf::Keyboard::Down) {
            idx = (idx + 1) % NUM_MENU_ITEMS;
        }
        else if (code == sf::Keyboard::Enter) {
            confirmSelection(idx);
            return;
        }
        menuLogic->selectItem(idx);
        updateButtonHighlight(idx);
    }

    void confirmSelection(int index) {
        menuLogic->confirm();
        finished = true;
        confirmedIndex = index;
    }

    void updateButtonHighlight(int index) {
        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            items[i].btn.setFillColor(i == index ? items[i].hoverColor : items[i].baseColor);
            items[i].targetScale = (i == index) ? HOVER_SCALE : 1.0f;
        }
    }

    void updateAnimations(double dt) {
        for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
            MenuItem& mi = items[i];
            float diff = mi.targetScale - mi.currScale;
            if (std::abs(diff) > 0.001f) {
                mi.currScale += diff * static_cast<float>(dt) * SCALE_SPEED;
                mi.btn.setScale(mi.currScale, mi.currScale);
                centerText(mi.lbl, mi.btn.getPosition());
            }
        }
    }

    void centerText(sf::Text& text, const sf::Vector2f& position) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2 + bounds.left, bounds.height / 2 + bounds.top);
        text.setPosition(position);
        text.setScale(items[0].currScale, items[0].currScale); // Sync with button scale
    }
};