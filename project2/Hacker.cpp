#include "Hacker.h"

// Constructor for Hacker class with one parameter: ID
Hacker::Hacker(int ID) {
	this->ID = ID;
}

// Hackers compare by commits, then hoodie queue entrance time, finally by ID
bool Hacker::operator < (const Hacker& other) const {
	if (abs(this->legitCommits - other.legitCommits) < 0.00001) {
		// legit commit numbers are equal
		if (abs(this->hoodieQueueEntranceTime - other.hoodieQueueEntranceTime) < 0.00001) {
			// they entered the hoodie queue at the same time
			return (this->ID - other.ID > 0.00001);
		}
		return (this->hoodieQueueEntranceTime - other.hoodieQueueEntranceTime > 0.00001);
	}
	return (other.legitCommits - this->legitCommits > 0.00001);
}

// Increase hacker's commit number and check if that commit has more than 20 lines
void Hacker::commit(int line) {
	this->totalLines += line;
	nofCommits++;
	if (line >= 20) {
		// this commit counts for sticker queue entrance attempt
		legitCommits++;
	}
}

// Get a sticker if there is an available desk, else get in sticker line
void Hacker::getSticker(float time, vector<pair<float, bool>>& stickerDesks, queue<Hacker*>& stickerQueue,priority_queue<Event>& events) {
	// This is the starting time for turnaround if hacker did not wait in line
	if (this->stickerQueueEntranceTime == -1) {
		this->stickerQueueEntranceTime = time;
	}
	for (int i = 0; i < stickerDesks.size(); i++) {
		pair<float, bool>* cur = &stickerDesks[i];
		// check if desk is available
		if (cur->second) {
			// this desk is not available anymore
			cur->second = false;
			this->occupyingDesk = i;
			this->timeSpentAtStickerQueue += time - this->stickerQueueEntranceTime; // add sticker queue wait time
			// create new event when this hacker get the sticker
			Event stickerServed = Event();
			stickerServed.ID = this->ID;
			stickerServed.time = time + cur->first;
			stickerServed.type = 4;
			events.push(stickerServed);
			return;
		}
	}
	// no desks available, hacker got in line
	this->stickerQueueEntranceTime = time;
	stickerQueue.push(this);
}

// Get a hoodie if there is an available desk, else get in hoodie line
void Hacker::getHoodie(float time, vector<pair<float, bool>>& hoodieDesks, priority_queue<Hacker*, vector<Hacker*>, CompareHackerPointers>& hoodieQueue, priority_queue<Event>& events) {
	// This is the starting time for turnaround if hacker did not wait in line
	if (this->hoodieQueueEntranceTime == -1) {
		this->hoodieQueueEntranceTime = time;
	}
	for (int i = 0; i < hoodieDesks.size(); i++) {
		pair<float, bool>* cur = &hoodieDesks[i];
		// check if desk is available
		if (cur->second) {
			cur->second = false;
			this->occupyingDesk = i;
			this->timeSpentAtHoodieQueue += time - this->hoodieQueueEntranceTime;
			// create new event when this hacker get the hoodie
			Event hoodieServed = Event();
			hoodieServed.ID = this->ID;
			hoodieServed.time = time + cur->first;
			hoodieServed.type = 5;
			events.push(hoodieServed);
			return;
		}
	}
	// no desks available, hacker got in line
	this->hoodieQueueEntranceTime = time;
	hoodieQueue.push(this);
}