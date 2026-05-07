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

    static int clampIndex(int idx, int size)
    {
        if (size <= 0) return 0;
        if (idx < 0) return 0;
        if (idx >= size) return size - 1;
        return idx;
    }

    // Count alive pets
    int countAlive(Player* p) const {
        if (!p) return 0;
        int cnt = 0;
        for (int i = 0; i < p->getTeamSize(); ++i)
        {
            Pet* pet = p->getPet(i);
            if (pet && pet->isAlive()) ++cnt;
        }
        return cnt;
    }

    // End battle when no alive pets on one side
    void checkEnd() {
        int humanAlive = countAlive(human);
        int aiAlive = countAlive(ai);
        if (humanAlive == 0 || aiAlive == 0) {
            battleOver = true;
            humanWon = (humanAlive > 0);
        }
    }

    // Auto-swap active if active pet died
    void autoSwap(Player* p, int& activeIndex) {
        if (!p) return;
        activeIndex = clampIndex(activeIndex, p->getTeamSize());
        Pet* active = p->getPet(activeIndex);
        if (active && active->isAlive()) return;
        for (int i = 0; i < p->getTeamSize(); ++i) {
            Pet* pet = p->getPet(i);
            if (pet && pet->isAlive()) {
                activeIndex = i;
                return;
            }
        }
    }

    // Randomly selects a duration between 8 and 14 seconds
    double getRandomSwapDuration() const {
        return 8.0 + (std::rand() % 7); // 8..14
    }

    // AI swaps its active pet with a random benched pet
    void aiSwapPet() {
        if (!ai) return;
        int currentActive = ai->getActivePetIndex();
        int possibleSwaps[MAX_TEAM_SIZE];
        int validSwapCount = 0;

        // Find valid swap candidates
        for (int i = 0; i < ai->getTeamSize(); ++i) {
            Pet* pet = ai->getPet(i);
            if (i != currentActive && pet && pet->isAlive()) {
                if (validSwapCount < MAX_TEAM_SIZE)
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
        aiNextSwapDuration = getRandomSwapDuration();
    }

    /// Called every frame
    void update(double dt) {
        if (battleOver) return;
        if (!human || !ai) { battleOver = true; humanWon = false; return; }

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
        if (!human || !ai) return;
        humanActive = clampIndex(humanActive, human->getTeamSize());
        aiActive = clampIndex(aiActive, ai->getTeamSize());
        Pet* h = human->getPet(humanActive);
        Pet* a = ai->getPet(aiActive);
        if (h && a && h->isAlive() && a->isAlive() && h->isReadyToAttack()) {
            h->performAttack(a);
            checkEnd();
        }
    }

    void humanUseAbility(int idx) {
        if (battleOver) return;
        if (!human || !ai) return;
        humanActive = clampIndex(humanActive, human->getTeamSize());
        aiActive = clampIndex(aiActive, ai->getTeamSize());
        Pet* h = human->getPet(humanActive);
        Pet* a = ai->getPet(aiActive);
        if (h && a && h->isAlive() && a->isAlive() && h->isReadyToAttack()) {
            h->useAbility(idx, a);
            checkEnd();
        }
    }

    /// Player chooses bench pet to swap in
    void humanSwap(int benchIndex) {
        if (!human) return;
        if (benchIndex < 0 || benchIndex >= human->getTeamSize()) return;
        Pet* p = human->getPet(benchIndex);
        if (!p || !p->isAlive()) return;
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