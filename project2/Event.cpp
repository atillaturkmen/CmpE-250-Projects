#include "Event.h"

bool Event::operator < (const Event& other) const {
	if (abs(this->time - other.time) < 0.00001) {
		// times are equal
		return (this->ID - other.ID > 0.00001); // For sticker queue entrance
	}
	return (this->time - other.time > 0.00001);
}