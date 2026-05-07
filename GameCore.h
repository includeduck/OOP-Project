#pragma once

#include "Player.h"
#include "AIPlayer.h"
#include "Unicorn.h"
#include <SFML/System/Sleep.hpp>
#include <cstdlib>
#include <ctime>

class GameCore {
private:
    Player* human;
    AIPlayer* ai;

    int humanActive;  // index of active human pet
    int aiActive;     // index of active AI pet

    bool battleOver;
    bool humanWon;

    double aiSwapTimer;        // Tracks elapsed time since last AI swap
    double aiNextSwapDuration; // Random duration until next AI swap

    // Count alive pets
    int countAlive(Player* p) const {
        int cnt = 0;
        for (int i = 0; i < p->getTeamSize(); ++i)
            if (p->getPet(i)->isAlive()) ++cnt;
        return cnt;
    }

    // End battle when no alive pets on one side
    void checkEnd() {
        if (countAlive(human) == 0 || countAlive(ai) == 0) {
            battleOver = true;
            humanWon = (countAlive(human) > 0);
        }
    }

    // Auto-swap active if active pet died
    void autoSwap(Player* p, int& activeIndex) {
        if (p->getPet(activeIndex)->isAlive()) return;
        for (int i = 0; i < p->getTeamSize(); ++i) {
            if (p->getPet(i)->isAlive()) {
                activeIndex = i;
                return;
            }
        }
    }

    // Randomly selects a duration between 8 and 14 seconds
    double getRandomSwapDuration() const {
        return 1.0 + (std::rand() % 3);
    }

    // AI swaps its active pet with a random benched pet
    void aiSwapPet() {
        int currentActive = ai->getActivePetIndex();
        int possibleSwaps[10]; // Array for benched pets (max size 10)
        int validSwapCount = 0;

        // Find valid swap candidates
        for (int i = 0; i < ai->getTeamSize(); ++i) {
            if (i != currentActive && ai->getPet(i) && ai->getPet(i)->isAlive()) {
                possibleSwaps[validSwapCount++] = i;
            }
        }

        if (validSwapCount == 0) return;

        int newActiveIndex = possibleSwaps[std::rand() % validSwapCount];

        aiActive = newActiveIndex;
        ai->setActivePetIndex(newActiveIndex);
    }

public:
    /// Players must have at least one pet
    GameCore(Player* _human, AIPlayer* _ai) : human(_human), ai(_ai), humanActive(0), aiActive(0), battleOver(false), humanWon(false), aiSwapTimer(0.0), aiNextSwapDuration(3.0)
    {
        std::srand(static_cast<unsigned>(std::time(0)));
        aiNextSwapDuration = getRandomSwapDuration();
    }

    /// Called every frame
    void update(double dt) {
        if (battleOver) return;

        // Update AI swap timer
        aiSwapTimer += dt;
        if (aiSwapTimer >= aiNextSwapDuration) {
            aiSwapPet(); // Perform AI pet swap
            aiSwapTimer = 0.0; // Reset the timer
            aiNextSwapDuration = getRandomSwapDuration(); // Set a new random duration
        }

        for (int i = 0; i < human->getTeamSize(); ++i) {
            Pet* pet = human->getPet(i);
            if (pet) {
                Unicorn* unicorn = dynamic_cast<Unicorn*>(pet);
                if (unicorn) {
                    unicorn->updateCooldown(dt, human);
                }
                else {
                    pet->updateCooldown(dt);
                }
            }
        }

        for (int i = 0; i < ai->getTeamSize(); ++i) {
            Pet* pet = ai->getPet(i);
            if (pet) {
                Unicorn* unicorn = dynamic_cast<Unicorn*>(pet);
                if (unicorn) {
                    unicorn->updateCooldown(dt, ai);
                }
                else {
                    pet->updateCooldown(dt);
                }
            }
        }

        // Update both players
        human->update(dt, ai);
        ai->update(dt, human);

        // Swap out dead active pets
        autoSwap(human, humanActive);
        autoSwap(ai, aiActive);

        // Update the active pet index for both players
        human->setActivePetIndex(humanActive);
        ai->setActivePetIndex(aiActive);

        // Check if the battle is over
        checkEnd();
    }

    // Human commands on active pet
    void humanAttack() {
        if (battleOver) return;
        Pet* h = human->getPet(humanActive);
        Pet* a = ai->getPet(aiActive);
        if (h->isAlive() && a->isAlive() && h->isReadyToAttack()) {
            h->performAttack(a);
            checkEnd();
        }
    }

    void humanUseAbility(int idx) {
        if (battleOver) return;
        Pet* h = human->getPet(humanActive);
        Pet* a = ai->getPet(aiActive);
        if (h->isAlive() && a->isAlive() && h->isReadyToAttack()) {
            h->useAbility(idx, a);
            checkEnd();
        }
    }

    /// Player chooses bench pet to swap in
    void humanSwap(int benchIndex) {
        if (benchIndex < 0 || benchIndex >= human->getTeamSize()) return;
        if (!human->getPet(benchIndex)->isAlive()) return;
        humanActive = benchIndex;
    }

    // Accessors
    int getHumanActive() const { return humanActive; }
    Player* getHumanPlayer() const { return human; }
    int getAIActive() const { return aiActive; }
    Pet* getHumanPet(int i) const { return human->getPet(i); }
    Pet* getAIPet(int i) const { return ai->getPet(i); }
    int getHumanTeamSize() const { return human->getTeamSize(); }
    int getAITeamSize() const { return ai->getTeamSize(); }
    bool isBattleOver() const { return battleOver; }
    bool didHumanWin() const { return humanWon; }
};