// -------------------------------
// Griffin.h
// -------------------------------
#pragma once
#include "Pet.h"

struct Adder
{
    double first, second;
	double operator()(double a, double b) {
		first = a;
		second = b;
        return a + b;
    }
};

class Griffin : public Pet {
    static const int ABILITY_SELFHARM = 0;
    static const int ABILITY_STUNANDDAMAGE = 1;
    Adder adder;

public:
	static const int TYPE_ID = 2;
    Griffin(const char* _name) : Pet(_name, 1700.0, 50.0, 2.0, 5.0) {}

    void useAbility(int abilityIndex, Pet* target) override
    {
        if (!isReadyToAttack()) return;
        if (abilityIndex == ABILITY_SELFHARM)
        {
			setHealth(getHealth() / 1.25);
			attackPower = adder(attackPower, 5);
			speed -= 0.05;
			attackInterval = 1.0 / speed;
        }
        else if (abilityIndex == ABILITY_STUNANDDAMAGE)
        {
			target->takeDamage(attackPower * 2);
            target->getStunned(10);
        }
        resetAttackTimer();
    }

	int getType() const override {
        return TYPE_ID;
	}

	Pet* clone() const override
	{
		return new Griffin(getName());
	}
};
