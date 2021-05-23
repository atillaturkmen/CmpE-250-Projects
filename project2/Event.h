#include <iostream>
using namespace std;

#ifndef EVENTS_H
#define EVENTS_H

class Event {

public:
    float time;
    int ID = 0;
    int type;
    int line;

    bool operator < (const Event& other) const;
};

#endif