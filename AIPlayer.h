#pragma once

#include "Player.h"
#include <cstdlib>
#include <ctime>

class AIPlayer : public Player
{
public:
    AIPlayer() : Player()
    {
        srand(static_cast<unsigned>(time(0))); // Seed the random number generator
    }

    void update(double dt, Player* enemy) override
    {
        // Update pet cooldowns
        for (int i = 0; i < getTeamSize(); ++i)
        {
            if (team[i] && team[i]->isAlive())
                team[i]->updateCooldown(dt);
        }

        // Combat logic
        Pet* self = getActivePet();
        Pet* target = enemy->getActivePet();

        if (!self || !self->isAlive() || !self->isReadyToAttack()) return;
        if (!target || !target->isAlive()) return;

        // Attack decision
        int roll = rand() % 100;
        if (roll < 70) // 70% chance for basic attack
        {
            self->performAttack(target);
        }
        else // 30% chance for random ability
        {
            int abilityIndex = rand() % 2;
            self->useAbility(abilityIndex, target);
        }
    }
};