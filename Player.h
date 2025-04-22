#pragma once

#include <Card.h>
#include <vector>

class Player
{
public:
	Player(int id);
	std::vector<Card*> hand;
	int id;
	void addCard(Card* card, bool netCall);
	void removeCard(Card* card, bool netCall);
};


