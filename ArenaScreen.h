// ArenaScreen.h
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <string>
#include "AssetUtils.h"
#include "UIScreen.h"
#include "GraphicsManager.h"
#include "GameCore.h"
#include "Dragon.h"
#include "Phoenix.h"
#include "Unicorn.h"
#include "Griffin.h"

static const int ARENA_STATE_FIGHTING = 0;
static const int ARENA_STATE_GAMEOVER = 1;

class ArenaScreen : public UIScreen
{
private:
    static const int MAX_ITEMS = 10; // Maximum number of items in the inventory
    sf::RectangleShape itemButtons[MAX_ITEMS]; // Buttons for items
    sf::Text itemLabels[MAX_ITEMS];           // Labels for item names
    int itemCount;
    GraphicsManager* graphics;
    GameCore* core;
    sf::Font* font;

    // Pet textures
    sf::Texture* texDragon;
    sf::Texture* texPhoenix;
    sf::Texture* texUnicorn;
    sf::Texture* texGriffin;
    sf::Texture* bgT;
    sf::Sprite   background;

    // Active sprites
    sf::Sprite       activeHuman;
    sf::Sprite       activeAI;

    // Bench sprites & highlights
    sf::Sprite       benchHuman[MAX_TEAM_SIZE];
    sf::RectangleShape benchSelHuman[MAX_TEAM_SIZE];
    sf::Sprite       benchAI[MAX_TEAM_SIZE];
    sf::RectangleShape benchSelAI[MAX_TEAM_SIZE];

    // Health bars
    sf::RectangleShape barBgHuman;
    sf::RectangleShape barHuman;
    sf::RectangleShape barBgAI;
    sf::RectangleShape barAI;

    // Buttons
    sf::RectangleShape btnAttack, btnAbility1, btnAbility2;
    sf::Text           lblAttack, lblA1, lblA2;
    sf::RectangleShape overlay;
    sf::RectangleShape panelBg;

    // Stats
    sf::Text humanStatsText;
    sf::Text aiStatsText;

    // Log
    sf::Text logText;
    float    logTimer;
    int      state;

    // Music
    sf::Music battleMusic;

public:
    ArenaScreen(GraphicsManager* gfx, GameCore* gc)
		: graphics(gfx), core(gc), logTimer(-1), state(ARENA_STATE_FIGHTING), itemCount(0)
    {
        // Music Loading

        if (openMusic(battleMusic, "assets/battle_music.ogg")) {
            battleMusic.setLoop(true);
            battleMusic.setVolume(50.0f);
            battleMusic.play();
        }

        font = new sf::Font();
        loadFont(*font, "assets/main_font.ttf");

        const float itemButtonX = 1000;
        const float itemButtonY = 600;
        const float itemSpacing = 60;

        Player* human = core ? core->getHumanPlayer() : nullptr;
        itemCount = human ? human->getInventoryCount() : 0;
        itemCount = std::min(itemCount, MAX_ITEMS);

        const auto win = graphics->getSize();
        const float panelW = 200.0f;
        const float panelX = win.x - (panelW + 550);
        const float panelY = 50.0f;
        const float btnH = 40.0f;
        const float btnSpacing = 10.0f;

        panelBg.setSize({ panelW, float(itemCount) * (btnH + btnSpacing) + btnSpacing });
        panelBg.setFillColor(sf::Color(20, 20, 30, 180));
        panelBg.setPosition(panelX, panelY - btnSpacing / 2);

        for (int i = 0; i < itemCount; ++i) {
            float y = panelY + i * (btnH + btnSpacing);
            // Button
            itemButtons[i].setSize({ panelW - 20, btnH });
            itemButtons[i].setPosition(panelX + 10, y);
            itemButtons[i].setFillColor(sf::Color(70, 70, 200, 200));
            // Label
            itemLabels[i].setFont(*font);
            Item* it = human ? human->getInventoryItem(i) : nullptr;
            itemLabels[i].setString(it ? it->getName() : "(empty)");
            itemLabels[i].setCharacterSize(18);
            // center text in the button
            sf::FloatRect b = itemLabels[i].getLocalBounds();
            itemLabels[i].setOrigin(b.left + b.width / 2, b.top + b.height / 2);
            itemLabels[i].setPosition(panelX + panelW / 2, y + btnH / 2);
        }

        // Load textures
        texDragon = new sf::Texture();
        loadTextureOrPlaceholder(*texDragon, "assets/dragon.png");
        texPhoenix = new sf::Texture();
        loadTextureOrPlaceholder(*texPhoenix, "assets/phoenix.png");
        texUnicorn = new sf::Texture();
        loadTextureOrPlaceholder(*texUnicorn, "assets/unicorn.png");
        texGriffin = new sf::Texture();
        loadTextureOrPlaceholder(*texGriffin, "assets/griffin.png");

        humanStatsText.setFont(*font);
        humanStatsText.setCharacterSize(18);
        humanStatsText.setFillColor(sf::Color::White);
        humanStatsText.setStyle(sf::Text::Regular);

        aiStatsText.setFont(*font);
        aiStatsText.setCharacterSize(18);
        aiStatsText.setFillColor(sf::Color::White);
        aiStatsText.setStyle(sf::Text::Regular);

        // Background setup
        bgT = new sf::Texture();
        loadTextureOrPlaceholder(*bgT, "assets/background.png");
        background.setTexture(*bgT);
        auto bgSize = bgT->getSize();
        if (bgSize.x > 0 && bgSize.y > 0)
        {
            background.setScale(
                1280.0f / float(bgSize.x),
                720.0f / float(bgSize.y)
            );
        }

        // Active positions
        activeHuman.setPosition(300, 232);
        activeAI.setPosition(800, 232);

        // Bench layout
        const float benchXHuman = 100;
        const float benchXAI = 1280 - 180;
        for (int i = 0; core && i < core->getHumanTeamSize(); ++i) {
            benchHuman[i].setTexture(*getTexture(core->getHumanPet(i)));
            benchHuman[i].setScale(0.5f, 0.5f);
            benchHuman[i].setPosition(benchXHuman, 100 + i * 120);
            benchSelHuman[i].setSize({ 100,100 });
            benchSelHuman[i].setPosition(benchXHuman, 100 + i * 120);
            benchSelHuman[i].setFillColor(sf::Color(0, 0, 0, 0));
            benchSelHuman[i].setOutlineThickness(2);
            benchSelHuman[i].setOutlineColor(sf::Color::Green);
        }
        for (int j = 0; core && j < core->getAITeamSize(); ++j) {
            benchAI[j].setTexture(*getTexture(core->getAIPet(j)));
			benchAI[j].setScale(0.5f, 0.5f);
            benchAI[j].setPosition(benchXAI, 100 + j * 120);
            benchSelAI[j].setSize({ 100,100 });
            benchSelAI[j].setPosition(benchXAI, 100 + j * 120);
            benchSelAI[j].setFillColor(sf::Color(0, 0, 0, 0));
            benchSelAI[j].setOutlineThickness(2);
            benchSelAI[j].setOutlineColor(sf::Color::Red);
        }

        // Health bars setup
        barBgHuman.setSize({ 300,25 });
        barBgHuman.setFillColor(sf::Color(50, 50, 50));
        barBgHuman.setPosition(300, 500);

        barHuman.setSize({ 300,25 });
        barHuman.setFillColor(sf::Color(100, 255, 100));
        barHuman.setPosition(300, 500);

        barBgAI.setSize({ 300,25 });
        barBgAI.setFillColor(sf::Color(50, 50, 50));
        barBgAI.setPosition(630, 500);

        barAI.setSize({ 300,25 });
        barAI.setFillColor(sf::Color(255, 100, 100));
        barAI.setPosition(630, 500);

        // Buttons setup
        setupButton(btnAttack, lblAttack, "Attack", 300, 600);
        setupButton(btnAbility1, lblA1, "Ability 1", 540, 600);
        setupButton(btnAbility2, lblA2, "Ability 2", 780, 600);

        // Log setup
        logText.setFont(*font);
        logText.setCharacterSize(24);
        logText.setFillColor(sf::Color::White);
        logText.setStyle(sf::Text::Bold);
    }

    ~ArenaScreen() override
    {
		battleMusic.stop();
        delete font;
        delete texDragon;
        delete texPhoenix;
        delete texUnicorn;
        delete texGriffin;
		delete bgT;
        //std::cout << "Arena Destroyed!\n";
    }

    void handleEvent(const sf::Event& e) override {
        if (state != ARENA_STATE_FIGHTING) return;
        if (!core) return;

        if (e.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f pos(e.mouseButton.x, e.mouseButton.y);

            for (int i = 0; i < itemCount; ++i) {
                if (itemButtons[i].getGlobalBounds().contains(pos)) {
                    Player* human = core->getHumanPlayer();
                    int activeIndex = core->getHumanActive();
                    if (human && human->useItem(i + 1, activeIndex)) {
                        setLog("Item Used!");
                        refreshItemUI();
                    }
                    else {
                        setLog("Failed to Use Item!");
                    }
                    break;
                }
            }

            // Handle bench clicks
            for (int i = 0; i < core->getHumanTeamSize(); ++i) {
                if (benchSelHuman[i].getGlobalBounds().contains(pos)) {
                    core->humanSwap(i);
                }
            }

            // Handle button clicks
            if (btnAttack.getGlobalBounds().contains(pos)) {
                core->humanAttack();
                setLog("Attack!");
            }
            if (btnAbility1.getGlobalBounds().contains(pos)) {
                core->humanUseAbility(0);
                setLog("Used Ability 1!");
            }
            if (btnAbility2.getGlobalBounds().contains(pos)) {
                core->humanUseAbility(1);
                setLog("Used Ability 2!");
            }
        }
    }

    void update(double dt) override
    {
        if (!core) return;
        core->update(dt);

        // Update active sprites
        Pet* hPet = core->getHumanPet(core->getHumanActive());
        Pet* aPet = core->getAIPet(core->getAIActive());
        if (hPet) activeHuman.setTexture(*getTexture(hPet));
        if (aPet) activeAI.setTexture(*getTexture(aPet));

        // Update health bars
        float humanHealthRatio = 0.0f;
        if (hPet && hPet->getMaxHealth() > 0.0)
            humanHealthRatio = float(hPet->getHealth() / hPet->getMaxHealth());
        if (humanHealthRatio < 0.0f) humanHealthRatio = 0.0f;
        if (humanHealthRatio > 1.0f) humanHealthRatio = 1.0f;
        barHuman.setSize({ 300.0f * humanHealthRatio, 25 });

        float aiHealthRatio = 0.0f;
        if (aPet && aPet->getMaxHealth() > 0.0)
            aiHealthRatio = float(aPet->getHealth() / aPet->getMaxHealth());
        if (aiHealthRatio < 0.0f) aiHealthRatio = 0.0f;
        if (aiHealthRatio > 1.0f) aiHealthRatio = 1.0f;
        barAI.setSize({ 300.0f * aiHealthRatio, 25 });

        // Update human stats text
        if (hPet)
        {
            humanStatsText.setString(
                "Health: " + std::to_string(int(hPet->getHealth())) + "/" + std::to_string(int(hPet->getMaxHealth())) + "\n" +
                "Attack: " + std::to_string(int(hPet->getAttackPower())) + "\n" +
                "Defense: " + std::to_string(int(hPet->getDefense())) + "\n" +
                "Speed: " + std::to_string(int(hPet->getSpeed()))
            );
        }
        else
        {
            humanStatsText.setString("Health: -\nAttack: -\nDefense: -\nSpeed: -");
        }
        humanStatsText.setPosition(50, 650); // Bottom-right corner for human stats

        // Update AI stats text
        if (aPet)
        {
            aiStatsText.setString(
                "Health: " + std::to_string(int(aPet->getHealth())) + "/" + std::to_string(int(aPet->getMaxHealth())) + "\n" +
                "Attack: " + std::to_string(int(aPet->getAttackPower())) + "\n" +
                "Defense: " + std::to_string(int(aPet->getDefense())) + "\n" +
                "Speed: " + std::to_string(int(aPet->getSpeed()))
            );
        }
        else
        {
            aiStatsText.setString("Health: -\nAttack: -\nDefense: -\nSpeed: -");
        }
        aiStatsText.setPosition(1000, 650); // Bottom-left corner for AI stats

        // Update log timer
        if (logTimer >= 0) {
            logTimer += dt;
            if (logTimer > 2.5f) logTimer = -1;
        }

        // Check battle state
        if (core->isBattleOver() && state == ARENA_STATE_FIGHTING) {
            state = ARENA_STATE_GAMEOVER;
            setLog(core->didHumanWin() ? "Victory!" : "Defeat!");
        }
    }

    void draw(GraphicsManager& g) override {
        g.clear();
        g.draw(background);

        // Draw benches
        for (int i = 0; i < core->getHumanTeamSize(); ++i) {
            Pet* pet = core->getHumanPet(i);
            if (pet && pet->isAlive()) {
                benchSelHuman[i].setFillColor(sf::Color(0, 0, 0, 0)); // Transparent
                benchSelHuman[i].setOutlineColor(sf::Color::Yellow);  // Yellow for alive pets
            }
            else {
                benchSelHuman[i].setFillColor(sf::Color(128, 128, 128, 200)); // Gray for dead pets
                benchSelHuman[i].setOutlineColor(sf::Color(128, 128, 128));   // Gray outline
            }
            g.draw(benchHuman[i]);
            g.draw(benchSelHuman[i]);
        }

        for (int j = 0; j < core->getAITeamSize(); ++j) {
            Pet* pet = core->getAIPet(j);
            if (pet && pet->isAlive()) {
                benchSelAI[j].setFillColor(sf::Color(0, 0, 0, 0)); // Transparent
                benchSelAI[j].setOutlineColor(sf::Color::Red);     // Red for alive pets
            }
            else {
                benchSelAI[j].setFillColor(sf::Color(128, 128, 128, 200)); // Gray for dead pets
                benchSelAI[j].setOutlineColor(sf::Color(128, 128, 128));   // Gray outline
            }
            g.draw(benchAI[j]);
            g.draw(benchSelAI[j]);
        }

        // Draw active pets
        g.draw(activeHuman);
        g.draw(activeAI);

        // Draw health bars
        g.draw(barBgHuman);
        g.draw(barHuman);
        g.draw(barBgAI);
        g.draw(barAI);

        // Draw buttons
        g.draw(btnAttack);
        g.draw(lblAttack);
        g.draw(btnAbility1);
        g.draw(lblA1);
        g.draw(btnAbility2);
        g.draw(lblA2);

        g.draw(humanStatsText);
        g.draw(aiStatsText);

        g.draw(panelBg);
        for (int i = 0; i < itemCount; ++i) {
            g.draw(itemButtons[i]);
            g.draw(itemLabels[i]);
        }

        // Draw log
        if (logTimer >= 0) {
            sf::FloatRect textBounds = logText.getLocalBounds();
            logText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                textBounds.top + textBounds.height / 2.0f);
            logText.setPosition(1280.0f / 2.0f, 150);
            g.draw(logText);
        }

        g.display();
    }


    bool isFinished() const override {
        return core ? core->isBattleOver() : true;
    }

private:
    sf::Texture* getTexture(Pet* p)
    {
        if (dynamic_cast<Dragon*>(p))  return texDragon;
        if (dynamic_cast<Phoenix*>(p)) return texPhoenix;
        if (dynamic_cast<Unicorn*>(p)) return texUnicorn;
        if (dynamic_cast<Griffin*>(p)) return texGriffin;
        return texDragon;
    }

    void refreshItemUI() {
        if (!core) { itemCount = 0; return; }
        Player* human = core->getHumanPlayer();
        itemCount = human ? human->getInventoryCount() : 0;
        itemCount = std::min(itemCount, MAX_ITEMS);

        const float panelW = 200.0f;         // width of our item panel
        const float panelX = graphics->getSize().x - (panelW + 550); // flush right
        const float panelY = 50.0f;         // start 50px from top
        const float btnH = 40.0f;           // button height
        const float btnSpacing = 10.0f;     // space between buttons

        // Update panel background size
        panelBg.setSize({ panelW, float(itemCount) * (btnH + btnSpacing) + btnSpacing });

        for (int i = 0; i < itemCount; ++i) {
            float y = panelY + i * (btnH + btnSpacing);

            // Update button
            itemButtons[i].setSize({ panelW - 20, btnH });
            itemButtons[i].setPosition(panelX + 10, y);
            itemButtons[i].setFillColor(sf::Color(70, 70, 200, 200));

            // Update label
            itemLabels[i].setFont(*font);
            Item* it = human ? human->getInventoryItem(i) : nullptr;
            itemLabels[i].setString(it ? it->getName() : "(empty)");
            itemLabels[i].setCharacterSize(18);

            // Center text in the button
            sf::FloatRect b = itemLabels[i].getLocalBounds();
            itemLabels[i].setOrigin(b.left + b.width / 2, b.top + b.height / 2);
            itemLabels[i].setPosition(panelX + panelW / 2, y + btnH / 2);
        }
    }

    void setupButton(sf::RectangleShape& btn, sf::Text& lbl, const char* text, float x, float y)
    {
        // Button shape
        btn.setSize({ 150,50 });
        btn.setPosition(x, y);
        btn.setFillColor(sf::Color(70, 70, 200, 200));
        btn.setOutlineThickness(2);
        btn.setOutlineColor(sf::Color::White);

        // Button text
        lbl.setFont(*font);
        lbl.setString(text);
        lbl.setCharacterSize(24);
        lbl.setFillColor(sf::Color::White);

        // Center text
        sf::FloatRect textBounds = lbl.getLocalBounds();
        lbl.setOrigin(textBounds.left + textBounds.width / 2.0f,
            textBounds.top + textBounds.height / 2.0f);
        lbl.setPosition(x + 75, y + 25); // Center in button
    }

    void setLog(const char* message) {
        logText.setString(message);
        logTimer = 0;
    }
};