// PetSelection.h
#pragma once

#include "Dragon.h"
#include "Phoenix.h"
#include "Unicorn.h"
#include "Griffin.h"
#include "Player.h"

static const int MAX_SELECTION = 3;
static const int NUM_PROTOTYPES = 4;

class PetSelection {
private:
    Pet* prototypes[NUM_PROTOTYPES];
    Pet* selected[MAX_SELECTION];
    int  selectedCount;

public:
    PetSelection() : selectedCount(0)
    {
        prototypes[0] = new Dragon("Dragon");
        prototypes[1] = new Phoenix("Phoenix");
        prototypes[2] = new Griffin("Griffin");
        prototypes[3] = new Unicorn("Unicorn");

        for (int i = 0; i < MAX_SELECTION; ++i) selected[i] = nullptr;
    }

    ~PetSelection()
    {
        for (int i = 0; i < NUM_PROTOTYPES; ++i) delete prototypes[i];
        for (int i = 0; i < selectedCount; ++i) delete selected[i];
    }

    int getPrototypeCount() const { return NUM_PROTOTYPES; }

    Pet* getPrototype(int idx) const
    {
        return (idx >= 0 && idx < NUM_PROTOTYPES) ? prototypes[idx] : nullptr;
    }

    bool selectPrototype(int idx)
    {
        if (selectedCount >= MAX_SELECTION) return false;
        Pet* proto = getPrototype(idx);
        if (!proto) return false;
        selected[selectedCount++] = proto->clone();
        return true;
    }

	bool deselectPrototype(int idx)
    {
		if (idx < 0 || idx >= selectedCount) return false;
		delete selected[idx];
		for (int i = idx; i < selectedCount - 1; ++i)
        {
			selected[i] = selected[i + 1];
		}
		selected[selectedCount - 1] = nullptr;
		--selectedCount;
		return true;
	}

	bool isSelected(int idx) const
    {
		for (int i = 0; i < selectedCount; ++i)
        {
			if (selected[i] == prototypes[idx]) return true;
		}
		return false;
	}

	bool isFull() const
    {
		return selectedCount >= MAX_SELECTION;
	}

	bool isEmpty() const
    {
		return selectedCount == 0;
	}

    int getPrototypeType(int idx)
    {
		if (idx < 0 || idx >= NUM_PROTOTYPES) return -1;
		return prototypes[idx]->getType();
    }

    int getSelectedCount() const
    {
        return selectedCount;
    }

    Pet* getSelected(int idx) const
    {
        return (idx >= 0 && idx < selectedCount) ? selected[idx] : nullptr;
    }

    void commitTo(Player* player)
    {
        for (int i = 0; i < selectedCount; ++i)
        {
            player->addPet(selected[i]);
            selected[i] = nullptr;
        }
        selectedCount = 0;
    }

    void reset()
    {
        for (int i = 0; i < selectedCount; ++i)
        {
            delete selected[i];
            selected[i] = nullptr;
        }
        selectedCount = 0;
    }
};