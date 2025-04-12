#pragma once

class TrackedObject
{
public:
    static int counter;

    TrackedObject()
    {
        counter++;
    }

    TrackedObject(const TrackedObject&)
    {
        counter++;
    }

    ~TrackedObject()
    {
        counter--;
    }
};