// -------------------------------
// Dragon.h
// -------------------------------
#pragma once
#include "Pet.h"

class Dragon : public Pet
{
    static const int ABILITY_FIREBALL = 0;
    static const int ABILITY_SCALE_ARMOR = 1;
public:
    static const int TYPE_ID = 0;
    Dragon(const char* _name) : Pet(_name, 1200.0, 25.0, 8.0, 3.0) {}

    void useAbility(int abilityIndex, Pet* target) override
    {
        if (!isReadyToAttack() || !isAlive()) return;
        if (abilityIndex == ABILITY_FIREBALL)
        {
            target->takeDamage(target->getHealth() / 3);
        }
        else if (abilityIndex == ABILITY_SCALE_ARMOR)
        {
            defense += 1.15;

        }
        resetAttackTimer();
    }

    int getType() const override
    {
        return TYPE_ID;
    }

    Pet* clone() const override
    {
		return new Dragon(getName());
    }
};