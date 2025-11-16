#ifndef EATER_H
#define EATER_H

class IEater
{
public:
    float Hunger = 0;
    float MaxHunger = 100;
    float HungerRate = 10; // Hunger increase per second
    float FeedLevel = 1;

    virtual void SizeUp() = 0;

    virtual bool Eat(float amount)
    {
        Hunger -= amount;
        if (Hunger < 0)
        {
            FeedLevel++;
            Hunger = 0;
            if (FeedLevel > 2)
            {
                FeedLevel = 1;
                return true;
            }
        }
        return false;
    }

    virtual bool UpdateHunger(float deltaTime)
    {
        Hunger += HungerRate * deltaTime;
        if (Hunger > MaxHunger)
        {
            FeedLevel--;
            if (FeedLevel < 0)
            {
                return true; // Starved
            }
        }
        return false;
    }
};

#endif // EATER_H