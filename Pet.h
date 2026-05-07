#pragma once
#include <cstring>

class Pet
{
protected:
    char *name;
    double health;
    double maxHealth;
    double attackPower;
    double defense;
    double speed;               // attacks per second
    double attackInterval;      // seconds between attacks
    double timeSinceLastAttack; // seconds

public:
    Pet(const char *_name,
        double _maxHealth,
        double _attackPower,
        double _defense,
        double _speed)
        : health(_maxHealth),
          maxHealth(_maxHealth),
          attackPower(_attackPower),
          defense(_defense),
          speed(_speed),
          timeSinceLastAttack(0.0)
    {
        attackInterval = 1.0 / speed;
        name = new char[std::strlen(_name) + 1];
        std::strcpy(name, _name);
    }

    virtual ~Pet() { delete[] name; }

    Pet(const Pet&) = delete;
    Pet& operator=(const Pet&) = delete;

    const char *getName() const { return name; }
    double getHealth() const { return health; }
    double getMaxHealth() const { return maxHealth; }
    double getAttackPower() const { return attackPower; }
    double getDefense() const { return defense; }
    double getSpeed() const { return speed; }
    double getAttackInterval() const { return attackInterval; }
    double getTimeSinceLastAttack() const { return timeSinceLastAttack; }

    void setName(const char *_name)
    {
        delete[] name;
        name = new char[std::strlen(_name) + 1];
        std::strcpy(name, _name);
    }

    void setHealth(double h) { health = (h < 0.0 ? 0.0 : (h > maxHealth ? maxHealth : h)); }
    void setMaxHealth(double mh)
    {
        maxHealth = mh;
        if (health > maxHealth)
            health = maxHealth;
    }
    void setAttackPower(double ap) { attackPower = ap; }
    void setDefense(double def) { defense = def; }
    void setSpeed(double sp)
    {
        speed = sp;
        attackInterval = (speed > 0.0 ? 1.0 / speed : 0.0);
    }
    void setAttackInterval(double ai)
    {
        attackInterval = ai;
        speed = (ai > 0.0 ? 1.0 / ai : 0.0);
    }
    void setTimeSinceLastAttack(double t) { timeSinceLastAttack = t; }
    void updateCooldown(double dt) { timeSinceLastAttack += dt; }
    bool isReadyToAttack() const { return timeSinceLastAttack >= attackInterval; }
    void resetAttackTimer() { timeSinceLastAttack = 0.0; }

    virtual void takeDamage(double amount)
    {
        double dmg = amount - defense;
        if (dmg < 1.0)
            dmg = 1.0;
        setHealth(health - dmg);
    }

    virtual void getStunned(double amount)
    {
        double stun = amount;
        if (stun < 1.0) stun = 1.0;
        setTimeSinceLastAttack(timeSinceLastAttack + stun);
    }

    // Phoenix Specific---------------------------
    virtual void setHasRevived(bool) {}
    virtual bool getHasRevived() const { return false; }
    virtual void setFdep(bool) {}
    virtual bool getFdep() const { return false; }
    //--------------------------------------------

    bool isAlive() const { return health > 0.0; }

    virtual void performAttack(Pet *target)
    {
        if (!isReadyToAttack() || !isAlive())
            return;
        if (!target) return;
        target->takeDamage(attackPower);
        resetAttackTimer();
    }

    virtual Pet *clone() const = 0;
    virtual int getType() const = 0;

    virtual void useAbility(int abilityIndex, Pet *target) = 0;
};