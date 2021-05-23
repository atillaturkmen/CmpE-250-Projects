#include "Character.h"

#include<fstream>

using namespace std;


string lastCasualty1 = "No casualty"; // Name of the last casualty from community 1
string lastCasualty2 = "No casualty"; // Name of the last casualty from community 2

// Finds a character by its name in a community
// If that is character is dead finds another one as described in project description
Character* findCharacter (string name, Character* community[]) {
    for (int i = 0; i < 5; i++) {
        if (community[i]->name != name) {
            continue;
        }
        if (community[i]->isAlive) {
            return community[i];
        }
        while (!community[i]->isAlive && i < 4) {
            i++;
        }
        if (i == 4 && !community[i]->isAlive) {
            while (!community[i]->isAlive) {
                i--;
            }
        }
        return community[i];
    }
}

// Finds hobbit in a community
Character* findHobbit (Character* community[]) {
    for (int i = 0; i < 5; i++) {
        if (community[i]->type == "Hobbit") {
            return community[i];
        }
    }
}

// Checks lost conditions for a community
bool hasLost (Character* community[]) {
    // Check if hobbit is dead
    if (!findHobbit(community)->isAlive) {
        return true;
    }
    // Check if hobbit is alone
    for (int i = 0; i < 5; i++) {
        if (community[i]->type != "Hobbit" && community[i]->isAlive) {
            break;
        }
        if (i == 4) {
            return true;
        }
    }
    return false;
}

void attack (Character* attacker, Character* defender, int turnNo) {
    // Calculate attack score
    int attack = attacker->attack;
    int defense = defender->defense;
    int attackScore = attack - defense;

    // Damage dealt
    if (attackScore > 0 && attackScore < defender->remainingHealth) {
        defender->remainingHealth -= attackScore;
    }
    // Defender is dead
    else if (attackScore > 0 && attackScore >= defender->remainingHealth) {
        defender->remainingHealth = 0;
        defender->isAlive = false;
        if (turnNo % 2 == 1) {
            lastCasualty2 = defender->name;
        }
        else {
            lastCasualty1 = defender->name;
        }
    }
}

// Check for special skills
void makeSpecialMove (Character* attacker, Character* defender, Character* community[], int turnNo) {
    // Elves can give half of their health to their hobbit, per 10 rounds
    if (attacker->type == "Elves" && attacker->nRoundsSinceSpecial >= 10) {
        int transferredHealth = attacker->remainingHealth / 2;
        attacker->remainingHealth -= transferredHealth;
        findHobbit(community)->remainingHealth += transferredHealth;
        attacker->nRoundsSinceSpecial = 0;
    }
    // Dwarfs can call another dwarf, per 20 rounds
    // Copy dwarf will attack now and will be deleted real dwarf will attack in its turn normally if opponent is not dead
    if (attacker->type == "Dwarfs" && attacker->nRoundsSinceSpecial >= 20) {
        Character* copyDwarf = new Character(*attacker);
        attack(copyDwarf, defender, turnNo);
        delete copyDwarf;
        attacker->nRoundsSinceSpecial = 0;
    }
    // Wizards can turn the last killed community member back to life with the initial health of the killed member, per 50 rounds.
    if (attacker->type == "Wizards" && attacker->nRoundsSinceSpecial >= 50) {
        string lastCasualty;
        // Check which community this wizard belongs to
        if (turnNo % 2 == 1) {
            lastCasualty = lastCasualty1;
        }
        else {
            lastCasualty = lastCasualty2;
        }
        // Does nothing if no casualties in team
        if (lastCasualty != "No casualty") {
            Character* resurrected;
            for (int i = 0; i < 5; ++i) {
                if (community[i]->name == lastCasualty) {
                    resurrected = community[i];
                }
            }
            // Fill its health
            resurrected->remainingHealth = resurrected->healthHistory[0];
            // Resurrect if it is dead
            if (!resurrected->isAlive) {
                resurrected->isAlive = true;
                resurrected->nRoundsSinceSpecial = 0;
            }
            attacker->nRoundsSinceSpecial = 0;
        }
    }
}

int main(int argc, char* argv[]) {

    string infile_name = argv[1];
    string outfile_name = argv[2];

    Character* community1[5];
    Character* community2[5];
    Character* community1Ordered[5];
    Character* community2Ordered[5];

    ifstream in;
    in.open(infile_name);

    int nofRounds;

    in >> nofRounds;

    // Read characters of community 1
    for (int i = 0; i < 5; i++) {
        string name;
        string type;
        int attackPoint;
        int defensePoint;
        int healthPoint;

        in >> name >> type >> attackPoint >> defensePoint >> healthPoint;

        Character* c = new Character(name, type, attackPoint, defensePoint, healthPoint, nofRounds);

        community1[i] = c;
        community1Ordered[i] = c;
    }

    // Read characters of community 2
    for (int i = 0; i < 5; i++) {
        string name;
        string type;
        int attackPoint;
        int defensePoint;
        int healthPoint;

        in >> name >> type >> attackPoint >> defensePoint >> healthPoint;

        Character* c = new Character(name, type, attackPoint, defensePoint, healthPoint, nofRounds);

        community2[i] = c;
        community2Ordered[i] = c;
    }

    // Sort both communities alphabetically
    for (int i = 0; i < 4; i++) {
    	for (int j = i; j < 5; j++) {
    		if (*community1Ordered[j] < *community1Ordered[i]) {
    			swap(community1Ordered[j], community1Ordered[i]);
    		}
    		if (*community2Ordered[j] < *community2Ordered[i]) {
    			swap(community2Ordered[j], community2Ordered[i]);
    		}
    	}
    }

    string winner = "Draw";
    long long turnNo;

    // Battle begins
    for (turnNo = 1; turnNo <= nofRounds; ++turnNo) {

        string attackerName;
        string defenderName;
        string special;

        // Read what happens this turn from the input
    	in >> attackerName >> defenderName >> special;

        // Attacking team and defending team changes every turn
        Character* attacker;
        Character* defender;

        // Community 1 attacks on odd numbered turns
        if (turnNo % 2 == 1) {
            attacker = findCharacter(attackerName, community1Ordered);
            defender = findCharacter(defenderName, community2Ordered);
            if (special == "SPECIAL") {
                makeSpecialMove(attacker, defender, community1Ordered, turnNo);
            }
        }
        // Community 2 attacks on even numbered turns
        else {
            attacker = findCharacter(attackerName, community2Ordered);
            defender = findCharacter(defenderName, community1Ordered);
            if (special == "SPECIAL") {
                makeSpecialMove(attacker, defender, community2Ordered, turnNo);
            }
        }

        attack(attacker, defender, turnNo);

        // Update healths and special counts
        for (int i = 0; i < 5; i++) {
            *(community1Ordered[i]->healthHistory + turnNo) = community1Ordered[i]->remainingHealth;
            community1Ordered[i]->nRoundsSinceSpecial += 1;
            *(community2Ordered[i]->healthHistory + turnNo) = community2Ordered[i]->remainingHealth;
            community2Ordered[i]->nRoundsSinceSpecial += 1;
        }

        // Check lost conditions for both teams
        if (hasLost(community1)) {
            winner = "Community-2";
            break;
        }
        if (hasLost(community2)) {
            winner = "Community-1";
            break;
        }
    }

    if (winner == "Draw") {
        turnNo--;
    }

    in.close();

    // Count number of casualties
    int nofCasualty = 0;
    for (int i = 0; i < 5; ++i) {
        if (!community1[i]->isAlive) {
            nofCasualty++;
        }
        if (!community2[i]->isAlive) {
            nofCasualty++;
        }
    }

    ofstream out;
    out.open(outfile_name);

    // Print the outcome
    out << winner << endl;
    out << turnNo << endl;
    out << nofCasualty << endl;

    // Print health history of team 1
    for (int i = 0; i < 5; i++) {
        out << community1[i]->name << " ";
        for (int j = 0; j <= turnNo; j++) {
            out << *(community1[i]->healthHistory + j) << " ";
        }
        out << endl;
    }
    // Print health history of team 2
    for (int i = 0; i < 5; i++) {
        out << community2[i]->name << " ";
        for (int j = 0; j <= turnNo; j++) {
            out << *(community2[i]->healthHistory + j) << " ";
        }
        out << endl;
    }

    out.close();

    return 0;
}