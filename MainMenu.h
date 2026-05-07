// MainMenu.h
#pragma once

static const int MENU_NEW_GAME = 0;
static const int MENU_TRAINING = 1;
static const int MENU_BATTLE = 2;
static const int MENU_SHOP = 3;
static const int MENU_EXIT = 4;
static const int NUM_MENU_ITEMS = 5;

class MainMenu {
private:
    const char* itemNames[NUM_MENU_ITEMS] =
    {
        "New Game",
        "Training Camp",
        "Battle!",
        "Shop",
        "Exit"
    };

    int highlightedIndex;
    bool confirmed;
    int  confirmedIndex;

public:
    MainMenu() : highlightedIndex(0), confirmed(false), confirmedIndex(-1) {}

    int getItemCount() const { return NUM_MENU_ITEMS; }

    const char* getItemName(int idx) const
    {
        return (idx >= 0 && idx < NUM_MENU_ITEMS) ? itemNames[idx] : "";
    }

    void selectItem(int idx)
    {
        if (idx >= 0 && idx < NUM_MENU_ITEMS)
        {
            highlightedIndex = idx;
        }
    }

    int getHighlightedIndex() const
    {
        return highlightedIndex;
    }

    void confirm() {
        confirmed = true;
        confirmedIndex = highlightedIndex;
    }

    bool isConfirmed() const {
        return confirmed;
    }

    int getConfirmedIndex() const {
        return confirmedIndex;
    }

    void reset() {
        confirmed = false;
        confirmedIndex = -1;
    }
};