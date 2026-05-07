// Shop.h
#pragma once

#include <iostream>
#include "Item.h"
#include "HealPotion.h"
#include "BuffPotion.h"

class Shop
{
public:
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
		    case 1: return 30.0;
		    case 2: return 75.0;
		    case 3: return 75.0;
		    default: return 0.0;
		}
	}

    Item* buyItem(int choice, double& playerCoins)
    {
        switch (choice)
        {
            case 1:
                if (playerCoins >= 30.0)
                {
                    playerCoins -= 30.0;
                    return new HealPotion("Small Heal Potion", "Restores 50 HP", 600, 50.0);
                }
                break;
            case 2:
                if (playerCoins >= 75.0)
                {
                    playerCoins -= 75.0;
                    return new BuffPotion("Attack Elixir", "Boosts attack by 20% for 3 hits", 75.0, TYPE_ATTACK_BUFF, 3, 3);
                }
                break;
            case 3:
                if (playerCoins >= 75.0)
                {
                    playerCoins -= 75.0;
                    return new BuffPotion("Defense Elixir", "Boosts defense by 20% for 3 hits", 75.0, TYPE_DEFENSE_BUFF, 5, 3);
                }
                break;
        }
        return nullptr;
    }
};