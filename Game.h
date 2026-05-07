#pragma once

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "GraphicsManager.h"
#include "UIScreen.h"
#include "MainMenu.h"
#include "MainMenuScreen.h"
#include "PetSelection.h"
#include "PetSelectionScreen.h"
#include "Shop.h"
#include "ShopScreen.h"
#include "TrainingCamp.h"
#include "TrainingCampScreen.h"
#include "Player.h"
#include "AIPlayer.h"
#include "GameCore.h"
#include "ArenaScreen.h"

class Game
{
private:
    // screen states
    static const int STATE_MENU = 0;
    static const int STATE_SELECT = 1;
    static const int STATE_SHOP = 2;
    static const int STATE_TRAIN = 3; 
    static const int STATE_ARENA = 4;
    static const int STATE_EXIT = 5;

    GraphicsManager gfx;
    int             state;
    sf::Clock       clock;
    sf::Event       evt;
    UIScreen*       current;

    // persistent logic
    MainMenu      menuLogic;
    PetSelection  psLogic;
    Shop          shopLogic;
    TrainingCamp  tcLogic;
    Player        human;
    AIPlayer      ai;

    GameCore* corePtr;

    void buildScreen()
    {
        // Clean up previous screen and core
        if (current)
        {
            delete current;
            current = nullptr;
        }
        if (corePtr)
        {
            delete corePtr;
            corePtr = nullptr;
        }

        switch (state)
        {
            case STATE_MENU:
                menuLogic.reset();
                current = new MainMenuScreen(&gfx, &menuLogic);
                break;

            case STATE_SELECT:
                human.reset();
                psLogic.reset();
                current = new PetSelectionScreen(&gfx, &psLogic, &human);
                break;

            case STATE_SHOP:
                current = new ShopScreen(&gfx, &shopLogic, &human);
                break;

            case STATE_TRAIN:
                current = new TrainingCampScreen(&gfx, &tcLogic, &human);
                break;

            case STATE_ARENA:
            {
                // heal all human pets back to max HP
                human.resetPets();

                // build a random AI team
                ai = AIPlayer();
                Pet* pool[4] =
                {
                    new Dragon("AI_D1"),
                    new Phoenix("AI_P1"),
                    new Unicorn("AI_U1"),
                    new Griffin("AI_G1")
                };
                for (int i = 0; i < 3; ++i)
                {
                    ai.addPet(pool[rand() % 4]);
                }
                for (int i = 0; i < 4; ++i)
                {
                    delete pool[i];
                    pool[i] = nullptr;
                }

                // create core & screen
                corePtr = new GameCore(&human, &ai);
                current = new ArenaScreen(&gfx, corePtr);
                break;
            }

            case STATE_EXIT: break;
        }
    }

public:
    Game() : gfx(1280, 720, "Magical Pets Kingdom (DDLC Edition)"), state(STATE_MENU), current(nullptr), corePtr(nullptr)
    {
        srand(static_cast<unsigned>(std::time(nullptr)));
        buildScreen();
    }

    ~Game()
    {
        delete current;
        delete corePtr;
    }

    void run()
    {
        while (gfx.isOpen() && state != STATE_EXIT)
        {
            // 1) events
            while (gfx.pollEvent(evt))
            {
                if (evt.type == sf::Event::Closed)
                {
                    gfx.getWindow().close();
                    state = STATE_EXIT;
                }
                current->handleEvent(evt);
            }

            // 2) update
            double dt = clock.restart().asSeconds();
            current->update(dt);

            // 3) draw
            current->draw(gfx);

            // 4) transition
            if (current->isFinished())
            {
                if (state == STATE_MENU)
                {
                    int choice = menuLogic.getConfirmedIndex();
                    if (choice == MENU_NEW_GAME)                                     state = STATE_SELECT;
                    else if (choice == MENU_SHOP)                                    state = STATE_SHOP;
                    else if (choice == MENU_BATTLE && human.getTeamSize() != 0)      state = STATE_ARENA;
                    else if (choice == MENU_TRAINING && human.getTeamSize() != 0)    state = STATE_TRAIN;
                    else                                                             state = STATE_MENU;
                    if (choice == MENU_EXIT) gfx.getWindow().close();
                }
                else if (state == STATE_ARENA)
                {
                    if (corePtr->didHumanWin()) { human.addCoins(100.0); }
                    else { human.addCoins(30.0); }
                    // Delay a bit before transitioning
					sf::sleep(sf::milliseconds(2000));
                    state = STATE_MENU;
                }
                else { state = STATE_MENU; }

                buildScreen();
            }
        }
    }
};