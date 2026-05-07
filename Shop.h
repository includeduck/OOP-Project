// Shop.h
#pragma once

#include <iostream>
#include "Item.h"
#include "HealPotion.h"
#include "BuffPotion.h"

class Shop
{
public:
    static constexpr double PRICE_HEAL = 30.0;
    static constexpr double PRICE_ATTACK = 75.0;
    static constexpr double PRICE_DEFENSE = 75.0;
    static constexpr double HEAL_AMOUNT = 50.0;
    static constexpr double BUFF_MULTIPLIER = 1.2; // +20%
    static constexpr int BUFF_DURATION = 3;

    void showItems() const
    {
        std::cout
            << "1. Small Heal Potion    - Restores 50 HP        (30 coins)\n"
            << "2. Attack Elixir        - +20% Attack (3 turns) (75 coins)\n"
            << "3. Defense Elixir       - +20% Defense (3 turns)(75 coins)\n";
    }

	double getItemPrice(int choice) const
	{
		switch (choice)
		{
		    case 1: return PRICE_HEAL;
		    case 2: return PRICE_ATTACK;
		    case 3: return PRICE_DEFENSE;
		    default: return 0.0;
		}
	}

    Item* buyItem(int choice, double& playerCoins)
    {
        switch (choice)
        {
            case 1:
                if (playerCoins >= PRICE_HEAL)
                {
                    playerCoins -= PRICE_HEAL;
                    return new HealPotion("Small Heal Potion", "Restores 50 HP", PRICE_HEAL, HEAL_AMOUNT);
                }
                break;
            case 2:
                if (playerCoins >= PRICE_ATTACK)
                {
                    playerCoins -= PRICE_ATTACK;
                    return new BuffPotion("Attack Elixir", "Boosts attack by 20% for 3 hits", PRICE_ATTACK, TYPE_ATTACK_BUFF, BUFF_MULTIPLIER, BUFF_DURATION);
                }
                break;
            case 3:
                if (playerCoins >= PRICE_DEFENSE)
                {
                    playerCoins -= PRICE_DEFENSE;
                    return new BuffPotion("Defense Elixir", "Boosts defense by 20% for 3 hits", PRICE_DEFENSE, TYPE_DEFENSE_BUFF, BUFF_MULTIPLIER, BUFF_DURATION);
                }
                break;
        }
        return nullptr;
    }
};