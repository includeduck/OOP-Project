#pragma once

#include <SFML/Graphics.hpp>
#include "UIScreen.h"
#include "GraphicsManager.h"
#include "PetSelection.h"
#include "Player.h"

static const int MAX_PROTOTYPES = 4;
static const float CARD_WIDTH = 240.0f;
static const float CARD_HEIGHT = 320.0f;
static const float CARD_SPACING = 50.0f;
static const float BUTTON_WIDTH = 200.0f;
static const float BUTTON_HEIGHT = 50.0f;

class PetSelectionScreen : public UIScreen {
private:
    GraphicsManager* graphics;
    PetSelection* selection;
    Player* player;

    sf::Font         font;
    sf::Texture      backgroundTex;
    sf::Sprite       background;

    struct ProtoCard {
        sf::RectangleShape background;
        sf::Sprite          sprite;
        sf::Text            name;
        sf::Text            stats;
        sf::RectangleShape  highlight;
        float               currentScale;
        float               targetScale;
    };
    ProtoCard protoCards[MAX_PROTOTYPES];
    sf::Texture texPrototypes[MAX_PROTOTYPES];

    sf::RectangleShape btnConfirm;
    sf::Text           lblConfirm;
    bool               confirmEnabled;
    bool               finished;
    int                selectedIndex;

public:
    PetSelectionScreen(GraphicsManager* gfx, PetSelection* sel, Player* plyr)
        : graphics(gfx)
        , selection(sel)
        , player(plyr)
        , confirmEnabled(false)
        , finished(false)
        , selectedIndex(-1)
    {
        loadResources();
        layoutUI();
    }

    ~PetSelectionScreen() override = default; // no dynamic deletes

    void handleEvent(const sf::Event& e) override {
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            handleClick({ float(e.mouseButton.x), float(e.mouseButton.y) });
        }
        else if (e.type == sf::Event::MouseMoved) {
            handleHover({ float(e.mouseMove.x), float(e.mouseMove.y) });
        }
    }

    void update(double dt) override {
        // Animate card scales
        int count = selection->getPrototypeCount();
        for (int i = 0; i < count; ++i) {
            ProtoCard& c = protoCards[i];
            float delta = c.targetScale - c.currentScale;
            if (std::abs(delta) > 0.001f) {
                c.currentScale += delta * float(dt) * SCALE_SPEED;
                c.background.setScale(c.currentScale, c.currentScale);
                c.sprite.setScale(c.currentScale * 0.7f, c.currentScale * 0.7f);
            }
        }
    }

    void draw(GraphicsManager& g) override {
        g.clear();
        g.draw(background);

        int count = selection->getPrototypeCount();
        for (int i = 0; i < count; ++i) {
            auto& c = protoCards[i];
            // drop shadow
            sf::RectangleShape shadow = c.background;
            shadow.move(4, 4);
            shadow.setFillColor(sf::Color(0, 0, 0, 100));
            g.draw(shadow);
            // card
            g.draw(c.background);
            g.draw(c.sprite);
            g.draw(c.name);
            g.draw(c.stats);
            g.draw(c.highlight);
        }

        g.draw(btnConfirm);
        g.draw(lblConfirm);
        g.display();
    }

    bool isFinished() const override {
        return finished;
    }

    int getSelectedIndex() const {
        return selectedIndex;
    }

private:
    void loadResources() {
        font.loadFromFile("assets/main_font.ttf");
        backgroundTex.loadFromFile("assets/selection_bg.jpg");
        for (int i = 0; i < MAX_PROTOTYPES; ++i)
        {
            const char* path = nullptr;
            switch (i)
            {
                case 0: path = "assets/dragon.png"; break;
                case 1: path = "assets/phoenix.png"; break;
                case 2: path = "assets/griffin.png"; break;
                case 3: path = "assets/unicorn.png"; break;
            }
            texPrototypes[i].loadFromFile(path);
        }
    }

    void layoutUI()
    {
        sf::Vector2u win = graphics->getSize();
        background.setTexture(backgroundTex);
        background.setScale(float(win.x) / backgroundTex.getSize().x, float(win.y) / backgroundTex.getSize().y);

        int count = selection->getPrototypeCount();
        float totalW = count * CARD_WIDTH + (count - 1) * CARD_SPACING;
        float startX = (win.x - totalW) / 2.0f;
        float y0 = 100.0f;

        for (int i = 0; i < count; ++i) {
            ProtoCard& c = protoCards[i];
            int type = selection->getPrototypeType(i);
            float x = startX + i * (CARD_WIDTH + CARD_SPACING);

            // initial scales
            c.currentScale = 1.0f;
            c.targetScale = 1.0f;

            // card bg
            c.background.setSize({ CARD_WIDTH, CARD_HEIGHT });
            c.background.setPosition(x, y0);
            c.background.setFillColor({ 40, 40, 60, 200 });
            c.background.setOutlineThickness(2);
            c.background.setOutlineColor(sf::Color::Transparent);
            c.background.setOrigin(0, 0);

            // sprite
            c.sprite.setTexture(texPrototypes[type]);
            sf::FloatRect b = c.sprite.getLocalBounds();
            c.sprite.setOrigin(b.width / 2, b.height / 2);
            c.sprite.setPosition(x + CARD_WIDTH / 2, y0 + 90);

            // name
            c.name = makeText(getName(type), 24, x + 16, y0 + 16, sf::Color::White);
            // stats
            char buf[64];
            Pet* proto = selection->getPrototype(type);
            std::snprintf(buf, 64, "HP: %.0f  ATK: %.0f", proto->getMaxHealth(), proto->getAttackPower());
            c.stats = makeText(buf, 18, x + 16, y0 + 200, sf::Color::Green);

            // highlight
            c.highlight.setSize({ CARD_WIDTH + 8, CARD_HEIGHT + 8 });
            c.highlight.setPosition(x - 4, y0 - 4);
            c.highlight.setFillColor(sf::Color::Transparent);
            c.highlight.setOutlineThickness(3);
            c.highlight.setOutlineColor(sf::Color::Transparent);
        }

        // confirm button
        btnConfirm.setSize({ BUTTON_WIDTH, BUTTON_HEIGHT });
        btnConfirm.setPosition(win.x / 2 - BUTTON_WIDTH / 2, win.y - BUTTON_HEIGHT - 40);
        updateConfirmAppearance();
        lblConfirm = makeText("Confirm Team", 24,
            btnConfirm.getPosition().x + BUTTON_WIDTH / 2,
            btnConfirm.getPosition().y + BUTTON_HEIGHT / 2,
            sf::Color::White,
            true);
    }

    void handleClick(const sf::Vector2f& pos) {
        int count = selection->getPrototypeCount();
        for (int i = 0; i < count; ++i) {
            if (protoCards[i].background.getGlobalBounds().contains(pos)) {
                if (selection->selectPrototype(i)) {
                    selectedIndex = i;
                    confirmEnabled = true;
                    for (int j = 0; j < count; ++j) {
                        protoCards[j].highlight.setOutlineColor(
                            (j == i) ? sf::Color::Blue : sf::Color::Transparent);
                    }
                    updateConfirmAppearance();
                }
                return;
            }
        }
        if (confirmEnabled && btnConfirm.getGlobalBounds().contains(pos)) {
            selection->commitTo(player);
            finished = true;
        }
    }

    void handleHover(const sf::Vector2f& pos) {
        int count = selection->getPrototypeCount();
        for (int i = 0; i < count; ++i) {
            bool over = protoCards[i].background.getGlobalBounds().contains(pos);
            protoCards[i].targetScale = over ? HOVER_SCALE : 1.0f;
        }
        bool hoverBtn = btnConfirm.getGlobalBounds().contains(pos);
        btnConfirm.setFillColor(hoverBtn && confirmEnabled ?
            sf::Color(100, 200, 100) : sf::Color(100, 100, 100));
    }

    void updateConfirmAppearance()
    {
        btnConfirm.setFillColor(confirmEnabled ? sf::Color(100, 300, 100) : sf::Color(100, 100, 100));
    }

    sf::Text makeText(const char* str, unsigned size, float x, float y, sf::Color col, bool centered = false)
    {
        sf::Text t;
        t.setFont(font);
        t.setString(str);
        t.setCharacterSize(size);
        t.setFillColor(col);
        t.setPosition(x, y);
        if (centered)
        {
            auto b = t.getLocalBounds();
            t.setOrigin(b.left + b.width / 2, b.top + b.height / 2);
        }
        return t;
    }

    const char* getName(int idx) const
    {
        switch (idx)
        {
            case 0: return "Dragon";
            case 1: return "Phoenix";
            case 2: return "Griffin";
            case 3: return "Unicorn";
        }
        return "";
    }
};