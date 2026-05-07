#pragma once
#include "Pet.h"

class Phoenix : public Pet {
    static const int ABILITY_HELL = 0; // FULL DEPLETION ABILITY
    static const int ABILITY_FIRESTORM = 1; // Example second ability

    bool hasRevived; // Tracks whether the revive ability has been used
    bool fulldepleteHP;

public:
    static const int TYPE_ID = 1;

    Phoenix(const char* _name)
        : Pet(_name, 1600.0, 28.0, 3.0, 3.0), hasRevived(false), fulldepleteHP(false) {
    }

    void useAbility(int abilityIndex, Pet* target) override {
        if (!isReadyToAttack()) return;

        if (abilityIndex == ABILITY_FIRESTORM) {
            target->takeDamage(attackPower * 3);
            target->getStunned(1000);
        }

        else if (abilityIndex == ABILITY_HELL and !fulldepleteHP) {
            target->setHealth(0);
            fulldepleteHP = true;
        }

        resetAttackTimer();
    }

	void setHasRevived(bool revived) override {
		hasRevived = revived;
	}

	bool getHasRevived() const override {
		return hasRevived;
	}

	void setFdep(bool f) override {
		fulldepleteHP = f;
	}

	bool getFdep() const override {
		return fulldepleteHP;
	}

    int getType() const override {
        return TYPE_ID;
    }

    Pet* clone() const override {
        return new Phoenix(getName());
    }

    // Override takeDamage to handle the revive ability
    void takeDamage(double amount) override {
        health -= amount;

        // Check if health drops to or below zero and revive hasn't been used
        if (health <= 0 && !hasRevived) {
            hasRevived = true;
            health = maxHealth * 0.5; // Restore 50% of max health
        }
        else if (health < 0) {
            health = 0; // Ensure health doesn't go below zero
        }
    }
};
