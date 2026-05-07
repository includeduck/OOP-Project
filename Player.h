#pragma once

#include "Pet.h"
#include "Item.h"
#include "Phoenix.h"

static const int MAX_TEAM_SIZE = 3;
static const int MAX_INVENTORY_SIZE = 10;

class Player
{
protected:
    // Pets
    Pet* team[MAX_TEAM_SIZE];
    int   teamSize;

    // Active pet index
    int activePetIndex;

    // Currency
    double coins;

    // Inventory of items
    Item* inventory[MAX_INVENTORY_SIZE];
    int   inventoryCount;

public:
    Player()
        : teamSize(0), activePetIndex(0), coins(1000.0), inventoryCount(0)
    {
        for (int i = 0; i < MAX_TEAM_SIZE; ++i)
            team[i] = nullptr;
        for (int i = 0; i < MAX_INVENTORY_SIZE; ++i)
            inventory[i] = nullptr;
    }

    virtual ~Player() {    
        for (int i = 0; i < teamSize; ++i)
        {
            delete team[i];
            team[i] = nullptr;
        }
        for (int i = 0; i < inventoryCount; ++i)
        {
            delete inventory[i];
            inventory[i] = nullptr;
        }
    }

    void reset() {
        for (int i = 0; i < teamSize; ++i) {
            delete team[i];
            team[i] = nullptr;
        }
        teamSize = 0;
        activePetIndex = 0;
    }

    /// Fully heal every pet and reset its attack cooldown.
    void resetPets() {
        for (int i = 0; i < teamSize; ++i) {
			if (dynamic_cast<Phoenix*>(team[i]))
			{
				Phoenix* p = dynamic_cast<Phoenix*>(team[i]);
				p->setFdep(false);
                p->setHasRevived(false);
			}
            Pet* p = team[i];
            p->setHealth(p->getMaxHealth());
            p->resetAttackTimer();
        }
    }

    // Team management
    bool addPet(Pet* p) {
        if (teamSize >= MAX_TEAM_SIZE) return false;
        team[teamSize++] = p->clone();
        return true;
    }


    int getTeamSize() const { return teamSize; }
    Pet* getPet(int idx) const { return (idx >= 0 && idx < teamSize) ? team[idx] : nullptr; }

    // Active pet management
    int getActivePetIndex() const { return activePetIndex; }

    void setActivePetIndex(int idx) {
        if (idx >= 0 && idx < teamSize && team[idx] && team[idx]->isAlive()) {
            activePetIndex = idx;
        }
    }

    Pet* getActivePet() const { return getPet(activePetIndex); }

    // Inventory management
    bool addItem(Item* item)
    {
        if (inventoryCount >= MAX_INVENTORY_SIZE) return false;
        inventory[inventoryCount++] = item;
        return true;
    }

    // Use an item at index on the specified pet (removes it from inventory)
    bool useItem(int index, int petIndex)
    {
        int idx = index - 1;
        if (idx < 0 || idx >= inventoryCount) return false;

        Pet* target = getPet(petIndex);
        if (!target) return false;

        inventory[idx]->applyEffect(target);

        delete inventory[idx];
        for (int i = idx; i < inventoryCount - 1; ++i) {
            inventory[i] = inventory[i + 1];
        }
        inventory[--inventoryCount] = nullptr;

        return true; // Indicate success
    }

    int getInventoryCount() const { return inventoryCount; }
    Item* getInventoryItem(int idx) const { return (idx >= 0 && idx < inventoryCount) ? inventory[idx] : nullptr; }
    double getCoins() const { return coins; }
    double& getCoinsRef() { return coins; }
    void addCoins(double amount) { coins += amount; }
    bool spendCoins(double amount) { if (coins < amount) return false; coins -= amount; return true; }

    virtual void update(double dt, Player*)
    {
        for (int i = 0; i < teamSize; ++i)
        {
            if (team[i] && team[i]->isAlive()) team[i]->updateCooldown(dt);
        }
    }
};
