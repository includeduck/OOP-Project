// ShopScreen.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "AssetUtils.h"
#include "UIScreen.h"
#include "GraphicsManager.h"
#include "Shop.h"
#include "Player.h"

// Screen states
static const int SHOP_STATE_BROWSING = 0;
static const int SHOP_STATE_FEEDBACK = 1;

// Number of shop items
static const int NUM_ITEMS = 3;

class ShopScreen : public UIScreen {
private:
    GraphicsManager* graphics;
    Shop* shopLogic;
    Player* player;

    // Font and UI text
    sf::Font* font;
    sf::Text         titleText;
    sf::Text         coinText;
    sf::Text         feedbackText;

    // Exit button
    sf::RectangleShape exitButton;
    sf::Text           exitLabel;
    bool               exitButtonGlowing;
    sf::Clock          exitButtonClock;

    // Background
    sf::Texture      bgTexture;
    sf::Sprite       bgSprite;

    // Item textures, sprites & labels
    sf::Texture      itemTextures[NUM_ITEMS];
    sf::Sprite       itemSprites[NUM_ITEMS];
    sf::Text         itemLabels[NUM_ITEMS];
    float            glowTimer[NUM_ITEMS];  // glow timers, <0 = no glow
    static constexpr float GLOW_DURATION = 0.2f; // seconds

    // Feedback state
    sf::Clock        feedbackTimer;
    int              state;
    bool             finishedFlag;

public:
    // Constructor + setup everything inline
    inline ShopScreen(GraphicsManager* gfx, Shop* shop, Player* plyr)
        : graphics(gfx)
        , shopLogic(shop)
        , player(plyr)
        , state(SHOP_STATE_BROWSING)
        , finishedFlag(false)
        , exitButtonGlowing(false)
    {
        // Load font once
        font = new sf::Font();
        loadFont(*font, "assets/main_font.ttf");

        // Background
        loadTextureOrPlaceholder(bgTexture, "assets/shop_bg.png");
        bgSprite.setTexture(bgTexture);
        auto winSize = graphics->getSize();
        auto texSize = bgTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0)
        {
            bgSprite.setScale(
                float(winSize.x) / float(texSize.x),
                float(winSize.y) / float(texSize.y)
            );
        }

        // Title
        titleText.setFont(*font);
        titleText.setCharacterSize(32);
        titleText.setString("Shop");
        titleText.setPosition(350.f, 20.f);

        // Coins
        coinText.setFont(*font);
        coinText.setCharacterSize(20);
        coinText.setPosition(20.f, 550.f);

        // Exit button
        exitButton.setSize({ 100.f, 50.f });
        exitButton.setPosition(1150.f, 20.f);
        exitLabel.setFont(*font);
        exitLabel.setCharacterSize(20);
        exitLabel.setString("Exit");
        exitLabel.setPosition(1170.f, 30.f);

        // Shop items
        const char* imgFiles[NUM_ITEMS] = {
            "assets/potion_small.png",
            "assets/elixir_attack.png",
            "assets/elixir_defense.png"
        };
        const char* names[NUM_ITEMS] = {
            "Small Heal Potion",
            "Attack Elixir",
            "Defense Elixir"
        };
        const double prices[NUM_ITEMS] = { 30.0, 75.0, 75.0 };

        for (int i = 0; i < NUM_ITEMS; ++i) {
            loadTextureOrPlaceholder(itemTextures[i], imgFiles[i]);
            itemSprites[i].setTexture(itemTextures[i]);
            itemSprites[i].setPosition(50.f, 100.f + i * 120.f);
            float scale = 0.4f;
            itemSprites[i].setScale(scale, scale);

            itemLabels[i].setFont(*font);
            itemLabels[i].setCharacterSize(18);
            itemLabels[i].setString(
                std::string(names[i]) + " - " + std::to_string((int)prices[i]) + " coins"
            );
            itemLabels[i].setPosition(
                itemSprites[i].getPosition().x + itemSprites[i].getGlobalBounds().width + 10.f,
                itemSprites[i].getPosition().y + itemSprites[i].getGlobalBounds().height / 2.f - 10.f
            );

            glowTimer[i] = -1.f;
        }

        // Feedback text
        feedbackText.setFont(*font);
        feedbackText.setCharacterSize(20);
        feedbackText.setPosition(300.f, 500.f);
    }

    // Destructor
    ~ShopScreen() override {
        delete font;
    }

    // Event handling
    void handleEvent(const sf::Event& e) override {
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
            finishedFlag = true;
            return;
        }
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f pos((float)e.mouseButton.x, (float)e.mouseButton.y);
            // Exit
            if (exitButton.getGlobalBounds().contains(pos)) {
                exitButtonGlowing = true;
                exitButtonClock.restart();
                finishedFlag = true;
                return;
            }
            // Items
            if (state == SHOP_STATE_BROWSING) {
                for (int i = 0; i < NUM_ITEMS; ++i) {
                    if (itemSprites[i].getGlobalBounds().contains(pos)) {
                        glowTimer[i] = 0.f;
                        itemSprites[i].setColor(sf::Color(200, 200, 255));
                        tryBuy(i + 1);
                    }
                }
            }
        }
    }

    // Update
    void update(double dt) override {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            if (glowTimer[i] >= 0.f) {
                glowTimer[i] += static_cast<float>(dt);
                if (glowTimer[i] >= GLOW_DURATION) {
                    glowTimer[i] = -1.f;
                    itemSprites[i].setColor(sf::Color::White);
                }
            }
        }
        if (exitButtonGlowing && exitButtonClock.getElapsedTime().asSeconds() > 0.1f)
            exitButtonGlowing = false;
        coinText.setString("Coins: " + std::to_string((int)(player ? player->getCoins() : 0)));
        if (state == SHOP_STATE_FEEDBACK && feedbackTimer.getElapsedTime().asSeconds() > 0.5f)
            state = SHOP_STATE_BROWSING;
    }

    // Draw
    void draw(GraphicsManager& g) override {
        g.clear();
        g.draw(bgSprite);
        g.draw(titleText);
        g.draw(coinText);
        for (int i = 0; i < NUM_ITEMS; ++i) {
            g.draw(itemSprites[i]);
            g.draw(itemLabels[i]);
        }
        if (state == SHOP_STATE_FEEDBACK)
            g.draw(feedbackText);
        exitButton.setFillColor(exitButtonGlowing ? sf::Color(255, 150, 150) : sf::Color(200, 100, 100));
        g.draw(exitButton);
        g.draw(exitLabel);
        g.display();
    }

    // Query
    bool isFinished() const override { return finishedFlag; }

private:
    // Purchase logic
    void tryBuy(int itemIndex) {
        if (!shopLogic || !player) { showFeedback("Shop error"); return; }
        Item* it = shopLogic->buyItem(itemIndex, player->getCoinsRef());
        if (it) {
            if (player->addItem(it)) showFeedback("Purchased!");
            else {
                delete it;
                showFeedback("Inventory Full!");
                player->addCoins(shopLogic->getItemPrice(itemIndex));
            }
        }
        else showFeedback("Not enough coins!");
    }

    // Feedback
    void showFeedback(const char* msg) {
        feedbackText.setString(msg);
        feedbackTimer.restart();
        state = SHOP_STATE_FEEDBACK;
    }
};