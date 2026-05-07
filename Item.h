// Item.h
#pragma once

#include <cstring>
#include "Pet.h"

// Effect type constants
static const int TYPE_HEAL = 0;
static const int TYPE_ATTACK_BUFF = 1;
static const int TYPE_DEFENSE_BUFF = 2;

class Item
{
protected:
    char* name;
    char* description;
    double price;
    int type;         // TYPE_HEAL, TYPE_ATTACK_BUFF, TYPE_DEFENSE_BUFF
    double magnitude; // heal amount or buff multiplier (e.g. 1.2 = +20%)
    int duration;     // duration in turns (if applicable)

public:
    Item(const char* _name,
        const char* _description,
        double _price,
        int _type,
        double _magnitude,
        int _duration) : price(_price), type(_type), magnitude(_magnitude), duration(_duration)
    {
        name = new char[std::strlen(_name) + 1];
        std::strcpy(name, _name);
        description = new char[std::strlen(_description) + 1];
        std::strcpy(description, _description);
    }

    virtual ~Item()
    {
        delete[] name;
        delete[] description;
    }

    // Getters
    const char* getName() const { return name; }
    const char* getDescription() const { return description; }
    double getPrice() const { return price; }
    int getType() const { return type; }
    double getMagnitude() const { return magnitude; }
    int getDuration() const { return duration; }

    // Clone & apply
    virtual Item* clone() const = 0;
    virtual void  applyEffect(Pet* target) = 0;
};
