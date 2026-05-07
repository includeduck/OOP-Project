// TrainingCampScreen.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "AssetUtils.h"
#include "UIScreen.h"
#include "GraphicsManager.h"
#include "TrainingCamp.h"
#include "Player.h"

// Screen states
static const int TC_STATE_MINIGAME = 0;
static const int TC_STATE_ALLOCATE = 1;
static const int TC_STATE_FINISHED = 2;

// Options
static const int MAX_STAT_OPTIONS = 4;

class TrainingCampScreen : public UIScreen
{
private:
    GraphicsManager* graphics;
    TrainingCamp* campLogic;
    Player* player;

    // Shared font
    sf::Font* font;

    // Minigame UI
    sf::Text         instruction;
    sf::Text         timerText;
    sf::Clock        clock;
    int              score;

    // Allocation UI
    sf::Text         header;
    sf::Text         pointsText;

    // Pet selection buttons & labels
    sf::RectangleShape petButtons[MAX_TEAM_SIZE];
    sf::Text           petLabels[MAX_TEAM_SIZE];

    // Stat selection buttons & labels
    sf::RectangleShape statButtons[MAX_STAT_OPTIONS];
    sf::Text           statLabels[MAX_STAT_OPTIONS];

    // Points spending controls
    sf::RectangleShape plusButton;
    sf::RectangleShape minusButton;
    sf::Text           spendText;
    int                pointsToSpend;

    // Confirm button
    sf::RectangleShape confirmButton;
    sf::Text           confirmLabel;

    // Flow control
    int                state;
    bool               finished;
    int                selectedPet;
    int                selectedStat;

    // Button glow states
    bool                plusButtonGlowing;
    bool                minusButtonGlowing;
    bool                confirmButtonGlowing;
    sf::Clock           plusButtonClock;
    sf::Clock           minusButtonClock;
    sf::Clock           confirmButtonClock;

    // Exit Button Functionality
    sf::RectangleShape exitButton;
    sf::Text           exitLabel;
    bool               exitButtonGlowing;
    sf::Clock          exitButtonClock;

public:
    TrainingCampScreen(GraphicsManager* gfx,
        TrainingCamp* camp,
        Player* plyr)
        : graphics(gfx)
        , campLogic(camp)
        , player(plyr)
        , score(0)
        , pointsToSpend(0)
        , state(TC_STATE_MINIGAME)
        , finished(false)
        , selectedPet(0)
        , selectedStat(0)
        , plusButtonGlowing(false)
        , minusButtonGlowing(false)
        , confirmButtonGlowing(false)
        , exitButtonGlowing(false)
    {
        // Load font
        font = new sf::Font();
        loadFont(*font, "assets/main_font.ttf");

        // Minigame texts
        instruction.setFont(*font);
        instruction.setCharacterSize(24);
        instruction.setString("Click as many times as you can in 5 seconds!");
        instruction.setPosition(50, 50);

        timerText.setFont(*font);
        timerText.setCharacterSize(20);
        timerText.setPosition(50, 100);

        // Allocation header
        header.setFont(*font);
        header.setCharacterSize(24);
        header.setString("Allocate Training Points");
        header.setPosition(50, 50);

        pointsText.setFont(*font);
        pointsText.setCharacterSize(20);
        pointsText.setPosition(50, 80);

        // Setup pet buttons
        for (int i = 0; i < MAX_TEAM_SIZE; ++i) {
            petButtons[i].setSize({ 250, 40 });
            petButtons[i].setPosition(50, 120 + i * 60);
            petButtons[i].setFillColor(sf::Color(100, 100, 100));
            petLabels[i].setFont(*font);
            petLabels[i].setCharacterSize(18);
            petLabels[i].setPosition(60, 130 + i * 60);
        }

        // Setup stat buttons
        for (int i = 0; i < MAX_STAT_OPTIONS; ++i) {
            statButtons[i].setSize({ 200, 40 });
            statButtons[i].setPosition(400, 120 + i * 60);
            statButtons[i].setFillColor(sf::Color(100, 100, 100));
            statLabels[i].setFont(*font);
            statLabels[i].setCharacterSize(18);
            statLabels[i].setPosition(410, 130 + i * 60);
        }

        // Plus/minus buttons
        plusButton.setSize({ 40, 40 });
        plusButton.setPosition(50, 320);
        plusButton.setFillColor(sf::Color(150, 150, 150));
        minusButton.setSize({ 40, 40 });
        minusButton.setPosition(100, 320);
        minusButton.setFillColor(sf::Color(150, 150, 150));
        spendText.setFont(*font);
        spendText.setCharacterSize(18);
        spendText.setPosition(150, 330);

        // Confirm button
        confirmButton.setSize({ 120, 50 });
        confirmButton.setPosition(950, 600);
        confirmButton.setFillColor(sf::Color(100, 200, 100));
        confirmLabel.setFont(*font);
        confirmLabel.setCharacterSize(18);
        confirmLabel.setString("Confirm");
		confirmLabel.setPosition(960, 610); // Centered in button

        // Exit button
        exitButton.setSize({ 120, 50 });
        exitButton.setPosition(200, 600);
        exitButton.setFillColor(sf::Color(200, 100, 100));
        exitLabel.setFont(*font);
        exitLabel.setCharacterSize(18);
        exitLabel.setString("Exit");
		exitLabel.setPosition(210, 610);

        // Start
        startMinigame();
    }

    ~TrainingCampScreen() override {
        delete font;
    }

    bool isFinished() const override { return finished; }

    void handleEvent(const sf::Event& e) override {
        if (!campLogic || !player) { finished = true; state = TC_STATE_FINISHED; return; }
        if (state == TC_STATE_MINIGAME) {
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                score++;
            }
        }
        else if (state == TC_STATE_ALLOCATE) {
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                auto pos = sf::Vector2f((float)e.mouseButton.x, (float)e.mouseButton.y);

                // Pet buttons
                for (int i = 0; i < player->getTeamSize(); ++i) {
                    if (petButtons[i].getGlobalBounds().contains(pos)) {
                        selectedPet = i;
                        break;
                    }
                }

                // Stat buttons
                for (int j = 0; j < MAX_STAT_OPTIONS; ++j) {
                    if (statButtons[j].getGlobalBounds().contains(pos)) {
                        selectedStat = j;
                        break;
                    }
                }

                // Plus button
                if (plusButton.getGlobalBounds().contains(pos) && campLogic->getAvailablePoints() > pointsToSpend) {
                    pointsToSpend++;
                    plusButtonGlowing = true;
                    plusButtonClock.restart();
                }

                // Minus button
                if (minusButton.getGlobalBounds().contains(pos) && pointsToSpend > 0) {
                    pointsToSpend--;
                    minusButtonGlowing = true;
                    minusButtonClock.restart();
                }

                // Confirm button
                if (confirmButton.getGlobalBounds().contains(pos)) {
                    spendPoints();
                    if (campLogic->getAvailablePoints() == 0) finishAndReturn();
                    confirmButtonGlowing = true;
                    confirmButtonClock.restart();
                }

                if (exitButton.getGlobalBounds().contains(pos)) {
                    finished = true; // Mark the screen as finished
                    state = TC_STATE_FINISHED;
                    exitButtonGlowing = true;
                    exitButtonClock.restart();
                }
            }
        }
    }

    void update(double dt) override {
        if (!campLogic || !player) { finished = true; state = TC_STATE_FINISHED; return; }
        if (state == TC_STATE_MINIGAME) {
            float elapsed = clock.getElapsedTime().asSeconds();
            float remaining = 5.0f - elapsed;
            if (remaining < 0) remaining = 0;
            timerText.setString("Time: " + std::to_string((int)remaining));
            if (elapsed >= 5.0f) {
                campLogic->addPoints(score);
                enterAllocateMode();
            }
        }

        // Update button glow states
        if (plusButtonGlowing && plusButtonClock.getElapsedTime().asSeconds() > 0.1f) {
            plusButtonGlowing = false;
        }
        if (minusButtonGlowing && minusButtonClock.getElapsedTime().asSeconds() > 0.1f) {
            minusButtonGlowing = false;
        }
        if (confirmButtonGlowing && confirmButtonClock.getElapsedTime().asSeconds() > 0.1f) {
            confirmButtonGlowing = false;
        }
        if (exitButtonGlowing && exitButtonClock.getElapsedTime().asSeconds() > 0.1f) {
            exitButtonGlowing = false;
        }
    }

    void draw(GraphicsManager& g) override {
        g.clear();
        if (!campLogic || !player) { g.display(); return; }
        if (state == TC_STATE_MINIGAME) {
            g.draw(instruction);
            g.draw(timerText);
        }
        else if (state == TC_STATE_ALLOCATE) {
            g.draw(header);
            pointsText.setString("Points left: " + std::to_string(campLogic->getAvailablePoints()));
            g.draw(pointsText);

            for (int i = 0; i < player->getTeamSize(); ++i) {
                petButtons[i].setFillColor(i == selectedPet ? sf::Color(150, 150, 200) : sf::Color(100, 100, 100));
                g.draw(petButtons[i]);
                Pet* pet = player->getPet(i);
                petLabels[i].setString(std::to_string(i + 1) + ". " + (pet ? pet->getName() : "(empty)"));
                g.draw(petLabels[i]);
            }

            const char* stats[MAX_STAT_OPTIONS] = { "Health","Attack","Defense","Speed" };
            for (int j = 0; j < MAX_STAT_OPTIONS; ++j) {
                statButtons[j].setFillColor(j == selectedStat ? sf::Color(150, 150, 200) : sf::Color(100, 100, 100));
                g.draw(statButtons[j]);
                statLabels[j].setString(std::to_string(j + 1) + ". " + stats[j]);
                g.draw(statLabels[j]);
            }

            // Draw spend controls with glow
            plusButton.setFillColor(plusButtonGlowing ? sf::Color(200, 200, 150) : sf::Color(150, 150, 150));
            minusButton.setFillColor(minusButtonGlowing ? sf::Color(200, 200, 150) : sf::Color(150, 150, 150));
            g.draw(plusButton);
            g.draw(minusButton);
            spendText.setString("Spend: " + std::to_string(pointsToSpend));
            g.draw(spendText);

            // Draw confirm button with glow
            confirmButton.setFillColor(confirmButtonGlowing ? sf::Color(150, 255, 150) : sf::Color(100, 200, 100));
            g.draw(confirmButton);
            g.draw(confirmLabel);

			// Exit button with glow
            exitButton.setFillColor(exitButtonGlowing ? sf::Color(255, 150, 150) : sf::Color(200, 100, 100));
            g.draw(exitButton);
            g.draw(exitLabel);
        }
        g.display();
    }

private:
    void startMinigame()
    {
        score = 0;
        clock.restart();
        state = TC_STATE_MINIGAME;
    }

    void enterAllocateMode()
    {
        state = TC_STATE_ALLOCATE;
    }

    void spendPoints()
    {
        if (!campLogic || !player) return;
        if (selectedPet < 0 || selectedPet >= player->getTeamSize()) return;
        if (selectedStat < 0 || selectedStat >= MAX_STAT_OPTIONS) return;
        if (campLogic->allocatePoints(player->getPet(selectedPet), selectedStat, pointsToSpend))
        {
            pointsToSpend = 0;
        }
    }

    void finishAndReturn()
    {
        finished = true;
        state = TC_STATE_FINISHED;
    }
};