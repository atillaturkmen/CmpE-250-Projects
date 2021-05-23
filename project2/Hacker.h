#include <vector>
#include <iostream>
#include <queue>

#include "Event.h"

using namespace std;

#ifndef HACKER_H
#define HACKER_H

class Hacker {

public:
	class CompareHackerPointers {
	public:
	    bool operator () (Hacker* left, Hacker* right) const {
        	return *left < *right;
		}
	};
    int ID;
    int nofGifts = 0;
    int nofCommits = 0;
    int legitCommits = 0;
    long long totalLines = 0;
    int invalidAttempts = 0;
    int invalidBecauseGreed = 0;
    int occupyingDesk = -1;
    float timeSpentAtStickerQueue = 0;
    float timeSpentAtHoodieQueue = 0;
    float stickerQueueEntranceTime = -1;
    float hoodieQueueEntranceTime = -1;

    Hacker(int ID);
    bool operator < (const Hacker& other) const;
    void commit(int line);
    void getSticker(float time, vector<pair<float, bool>>& stickerDesks, queue<Hacker*>& stickerQueue,priority_queue<Event>& events);
    void getHoodie(float time, vector<pair<float, bool>>& hoodieDesks, priority_queue<Hacker*, vector<Hacker*>, CompareHackerPointers>& hoodieQueue, priority_queue<Event>& events);
};

#endif