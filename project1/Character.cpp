#include "Character.h"

Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {
    this->name = _name;
    this->type = _type;
    this->attack = _attack;
    this->defense = _defense;
    this->remainingHealth = _remainingHealth;
    this->nMaxRounds = _nMaxRounds;
    this->healthHistory = new int[_nMaxRounds];
    this->healthHistory[0] = _remainingHealth;
    this->nRoundsSinceSpecial = 0;
}

Character::Character(const Character& character) {
	this->name = character.name;
    this->type = character.type;
    this->attack = character.attack;
    this->defense = character.defense;
    this->remainingHealth = character.remainingHealth;
    this->nMaxRounds = character.nMaxRounds;
    this->healthHistory = new int[character.nMaxRounds];
    for (int i = 0; i < character.nMaxRounds; ++i) {
        this->healthHistory[i] = character.healthHistory[i];
    }
    this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
}

Character& Character::operator=(const Character& character) {
    if (&character == this) {
        return *this;
    }
    delete[] this->healthHistory;
    this->name = character.name;
    this->type = character.type;
    this->attack = character.attack;
    this->defense = character.defense;
    this->remainingHealth = character.remainingHealth;
    this->nMaxRounds = character.nMaxRounds;
    this->healthHistory = new int[character.nMaxRounds];
    for (int i = 0; i < character.nMaxRounds; ++i) {
        this->healthHistory[i] = character.healthHistory[i];
    }
    this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
}

bool Character::operator<(const Character& other) {
	// Get length of the shorter name
    int length = 0;
    bool otherIsLonger;
    if (this->name.length() > other.name.length()) {
        length = other.name.length();
        otherIsLonger = false;
    }
    else {
        length = this->name.length();
        otherIsLonger = true;
    }
    // Iterate until letters are different
    for (int i = 0; i < length; i++) {
    	if (this->name.at(i) == other.name.at(i)) {
    		continue;
    	}
        // Compare ASCII values of first different letters
    	return (this->name.at(i) < other.name.at(i));
    }
    // Return false if other is longer
    return !otherIsLonger;
}

Character::~Character() {
    delete[] this->healthHistory;
}