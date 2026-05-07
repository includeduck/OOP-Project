// TrainingCamp.h
#pragma once

#include "Pet.h"

// Stat constants
static const int TRAIN_HEALTH = 0;
static const int TRAIN_ATTACK = 1;
static const int TRAIN_DEFENSE = 2;
static const int TRAIN_SPEED = 3;

class TrainingCamp
{
private:
    int availablePoints;    // points not yet spent

public:
    TrainingCamp() : availablePoints(0) {}

    void addPoints(int newPoints) { availablePoints += newPoints; }
    int getAvailablePoints() const { return availablePoints; }

    bool allocatePoints(Pet* pet, int statType, int pointsToSpend)
    {
        if (!pet || pointsToSpend <= 0 || pointsToSpend > availablePoints) return false;

        switch (statType)
        {
            case TRAIN_HEALTH:
                pet->setMaxHealth(pet->getMaxHealth() + pointsToSpend * 10.0);
                break;
            case TRAIN_ATTACK:
                pet->setAttackPower(pet->getAttackPower() + pointsToSpend * 2.0);
                break;
            case TRAIN_DEFENSE:
                pet->setDefense(pet->getDefense() + pointsToSpend * 1.5);
                break;
            case TRAIN_SPEED:
                pet->setSpeed(pet->getSpeed() + pointsToSpend * 0.2);
                break;
            default: return false;
        }

        availablePoints -= pointsToSpend;
        return true;
    }
};