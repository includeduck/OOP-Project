// HealPotion.h
#pragma once
#include "Item.h"

class HealPotion : public Item
{
public:
    HealPotion(const char* _name, const char* _description, double _price, double _amount) : Item(_name, _description, _price, TYPE_HEAL, _amount, 0) {}
    Item* clone() const override { return new HealPotion(name, description, price, magnitude); }

    void applyEffect(Pet* target) override
    {
        double newHP = target->getHealth() + magnitude;
        target->setHealth(newHP);
    }
};
