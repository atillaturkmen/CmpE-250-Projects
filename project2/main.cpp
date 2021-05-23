#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <utility> // pair
#include <iomanip> // rounding decimals

#include "Event.h"
#include "Hacker.h"

using namespace std;

float g_time = 0; // time of simulation
vector <Hacker> hackers; // vector to hold incoming hackers
vector <pair<float, bool>> stickerDesks; // holds sticker desks
vector <pair<float, bool>> hoodieDesks; // holds hoodie desks
queue <Hacker*> stickerQueue; // holds hackers waiting in sticker queue
priority_queue <Hacker*, vector<Hacker*>, Hacker::CompareHackerPointers> hoodieQueue; // holds hackers waiting in hoodie queue
vector <Event> initialEvents; // this vector contains initial events, then a priority queue is created from it

int main(int argc, char* argv[]) {

	int maxStickerQueue = 0; // maximum length of sticker queue
	int maxHoodieQueue = 0; // maximum length of hoodie queue
	float totalTurnAroundTime = 0; // total turn around time of all hackers

	// read name of input and output files
	string infile_name = argv[1];
    string outfile_name = argv[2];

    // open input stream
    ifstream in;
    in.open(infile_name);

    // Read hacker arrival events
    int n;
    in >> n;
    for (int i = 0; i < n; ++i) {

    	float hackerEntrancetime;
    	in >> hackerEntrancetime;

    	Event arrival = Event();
    	arrival.time = hackerEntrancetime;
    	arrival.type = 1;

    	initialEvents.push_back(arrival);
    }

    // Read commit events
    int c;
    in >> c;
    for (int i = 0; i < c; i++) {

    	int ID;
    	int line;
    	float commitTime;
    	in >> ID >> line >> commitTime;

    	Event commit = Event();
    	commit.time = commitTime;
    	commit.ID = ID;
    	commit.line = line;
    	commit.type = 2;

    	initialEvents.push_back(commit);
    }

    // Read attempt events
    int q;
    in >> q;
    for (int i = 0; i < q; i++) {

    	int ID;
    	float attemptTime;
    	in >> ID >> attemptTime;

    	Event attempt = Event();
    	attempt.time = attemptTime;
    	attempt.ID = ID;
    	attempt.type = 3;

    	initialEvents.push_back(attempt);
    }

    // Read sticker desks
    int ds;
    in >> ds;
    for (int i = 0; i < ds; i++) {

    	float deskServiceTime;
    	in >> deskServiceTime;

    	stickerDesks.push_back(make_pair(deskServiceTime, true));
    }

    // Read queue desks
    int dh;
    in >> dh;
    for (int i = 0; i < dh; i++) {

    	float deskServiceTime;
    	in >> deskServiceTime;

    	hoodieDesks.push_back(make_pair(deskServiceTime, true));
    }

    in.close();

    // create an events heap and clear initial events
    priority_queue <Event> events(less<Event>(), initialEvents);
    initialEvents.clear();

    // Start simulation
    while (!events.empty()) {

    	Event currentEvent = events.top(); // get next event
    	events.pop(); // remove this event from priority queue
    	g_time = currentEvent.time; // advance time
    	int ID = currentEvent.ID; // get ID of the hacker that participates in event

    	switch (currentEvent.type) {
    		// Hacker arrival event
    		case 1:
    			{
    				hackers.push_back(Hacker(hackers.size() + 1));
    			}
    			break;
    		// Commit event
    		case 2:
    			{
    				if (hackers.size() < ID) {
    					continue; // no hacker with such ID
    				}
    				hackers[ID - 1].commit(currentEvent.line);
    			}
    			break;
    		// Sticker queue attempt event
    		case 3:
    			{
    				if (hackers.size() < ID) {
    					// no hacker with such ID
    					continue;
    				}
    				Hacker* cur = &hackers[ID - 1];
					if (cur->nofGifts == 3) {
						// attempted fourth time
						cur->invalidBecauseGreed++;
					}
					else if (cur->legitCommits < 3) {
						// commited less than three times
						cur->invalidAttempts++;
					}
					else if (stickerQueue.empty()) {
						// line is empty hacker will try to get a sticker
						cur->getSticker(g_time, stickerDesks, stickerQueue, events);
					}
					else {
						// line is not empty hacker got in line
						cur->stickerQueueEntranceTime = g_time;
						stickerQueue.push(cur);
					}
    			}
    			break;
    		// Sticker served event
    		case 4:
    			{
					Hacker* cur = &hackers[ID - 1];
					if (cur->occupyingDesk >= 0) {
						// this desk is not occupied anymore
						stickerDesks[cur->occupyingDesk].second = true;
					}
					cur->occupyingDesk = -1;
					if (hoodieQueue.empty()) {
						// hoodie line is empty hacker will try to get a hoodie
						cur->getHoodie(g_time, hoodieDesks, hoodieQueue, events);
					}
					else {
						// hacker got in line
						cur->hoodieQueueEntranceTime = g_time;
						hoodieQueue.push(cur);
					}
					// serve next hacker if sticker line is not empty
					if (!stickerQueue.empty()) {
						stickerQueue.front()->getSticker(g_time, stickerDesks, stickerQueue, events);
						stickerQueue.pop();
					}
    			}
    			break;
    		// Hoodie served event
    		case 5:
    			{
					Hacker* cur = &hackers[ID - 1];
					cur->nofGifts++; // increment gift number of this hacker
					totalTurnAroundTime += g_time - cur->stickerQueueEntranceTime; // add turnaround time
					cur->stickerQueueEntranceTime = -1; // reset sticker queue entrance
					cur->hoodieQueueEntranceTime = -1; // reset hoodie queue entrance
					if (cur->occupyingDesk >= 0) {
						// this desk is not occupied anymore
						hoodieDesks[cur->occupyingDesk].second = true;
					}
					cur->occupyingDesk = -1;
					if (!hoodieQueue.empty()) {
						// serve next hacker if hoodie queue is not empty
						hoodieQueue.top()->getHoodie(g_time, hoodieDesks, hoodieQueue, events);
						hoodieQueue.pop();
					}
    			}
    			break;
    	}
    	// update max queue sizes
    	if (stickerQueue.size() > maxStickerQueue) {
    		maxStickerQueue = stickerQueue.size();
    	}
    	if (hoodieQueue.size() > maxHoodieQueue) {
    		maxHoodieQueue = hoodieQueue.size();
    	}
    }

    // Calculate results

    int nofHackers = hackers.size();
    long long totalGifts = 0;
    float totalStickerWait = 0;
    float totalHoodieWait = 0;
    long long totalCommit = 0;
    long long totalLine = 0;
    long long totalInvalid = 0;
    long long totalInvalidBecauseGreed = 0;
    int maxQueueWaiter = -2;
    float maxQueueWaiterTime = -2;
    int minQueueWaiter = -1;
    float minQueueWaiterTime = 2000000000;

    for (auto hacker : hackers) {
    	totalGifts += hacker.nofGifts;
    	totalStickerWait += hacker.timeSpentAtStickerQueue;
    	totalHoodieWait += hacker.timeSpentAtHoodieQueue;
    	totalCommit += hacker.nofCommits;
    	totalLine += hacker.totalLines;
    	totalInvalid += hacker.invalidAttempts;
    	totalInvalidBecauseGreed += hacker.invalidBecauseGreed;
    	float timeSpentAtQueues = hacker.timeSpentAtHoodieQueue + hacker.timeSpentAtStickerQueue;
    	if (timeSpentAtQueues - maxQueueWaiterTime > 0.00001) {
    		maxQueueWaiter = hacker.ID;
    		maxQueueWaiterTime = timeSpentAtQueues;
    	}
    	if (hacker.nofGifts == 3 && (minQueueWaiterTime - timeSpentAtQueues) > 0.00001) {
    		minQueueWaiter = hacker.ID;
    		minQueueWaiterTime = timeSpentAtQueues;
    	}
    }

    // If there are no hacker that got three presents
    if (minQueueWaiter == -1) {
    	minQueueWaiterTime = -1;
    }

    float averageGifts = (float)totalGifts / nofHackers;
    float averageStickerWait = totalStickerWait / totalGifts;
    float averageHoodieWait = totalHoodieWait / totalGifts;
    float averageCommits = (float)totalCommit / nofHackers;
    float averageLines = (float)totalLine / totalCommit;
    float averageTurnAround = totalTurnAroundTime / totalGifts;

    ofstream out;
    out.open(outfile_name);

    out << fixed;
    out << setprecision(3);

    out << maxStickerQueue << endl;
    out << maxHoodieQueue << endl;
    out << averageGifts << endl;
    out << averageStickerWait << endl;
    out << averageHoodieWait << endl;
    out << averageCommits << endl;
    out << averageLines << endl;
    out << averageTurnAround << endl;
    out << totalInvalid << endl;
    out << totalInvalidBecauseGreed << endl;
    out << maxQueueWaiter << " " << maxQueueWaiterTime << endl;
    out << minQueueWaiter << " " << minQueueWaiterTime << endl;
    out << g_time;

    out.close();

    return 0;
}