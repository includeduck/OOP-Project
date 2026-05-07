// BuffPotion.h  (inline implementation)
#pragma once
#include "Item.h"

class BuffPotion : public Item
{
public:
    BuffPotion(const char* _name,
        const char* _description,
        double _price,
        int _buffType,
        double _multiplier,
        int _duration) : Item(_name, _description, _price, _buffType, _multiplier, _duration) {}

    Item* clone() const override { return new BuffPotion(name, description, price, type, magnitude, duration); }

    void applyEffect(Pet* target) override
    {
        if (!target) return;
        if (type == TYPE_ATTACK_BUFF)
        {
            double newAP = target->getAttackPower() * magnitude;
            target->setAttackPower(newAP);
        }
        else if (type == TYPE_DEFENSE_BUFF)
        {
            double newDef = target->getDefense() * magnitude;
            target->setDefense(newDef);
        }
    }
};
