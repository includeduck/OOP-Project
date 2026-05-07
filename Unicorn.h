#pragma once
#include "Pet.h"
#include "Player.h"

class Unicorn : public Pet {
    static const int ABILITY_HEALSELF = 0;
    static const int ABILITY_HORN = 1;

public:
    static const int TYPE_ID = 3;

    Unicorn(const char* _name) : Pet(_name, 3000.0, 15.0, 20.5, 3.33) {}

    void useAbility(int abilityIndex, Pet* target) override
    {
        if (!isReadyToAttack()) return;

        if (abilityIndex == ABILITY_HEALSELF)
        {
			setHealth(getHealth() + getMaxHealth() * 0.3);
			if (getHealth() > getMaxHealth())
            {
				setHealth(getMaxHealth());
			}
        }

        if (abilityIndex == ABILITY_HORN)
        {
            target->takeDamage(attackPower + 1);
            target->setHealth(target->getHealth() * 0.75);
        }

        resetAttackTimer();
    }

    int getType() const override {
        return TYPE_ID;
    }

    Pet* clone() const override {
        return new Unicorn(getName());
    }

    void updateCooldown(double dt, Player* player) {
        Pet::updateCooldown(dt);

        // Get the active pet
        Pet* activePet = player->getPet(player->getActivePetIndex());

        // Heal the active pet if the Unicorn is not the active pet
        if (activePet != this && activePet->isAlive() && this->isAlive())
        {
            double healAmount = activePet->getMaxHealth() * 0.005;
            activePet->setHealth(activePet->getHealth() + healAmount);

            if (activePet->getHealth() > activePet->getMaxHealth())
            {
                activePet->setHealth(activePet->getMaxHealth());
            }
        }
    }
};