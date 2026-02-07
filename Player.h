#pragma once

#include <Card.h>
#include <vector>
#include <string>

enum PlayerStatus
{
	ATTACKER1,
	ATTACKER2,
	DEFENDER,
	FINISHED,
	NONE
};

class Player
{
public:
	Player(int id);
	std::vector<Card*> hand;
	int id;
	int finalPos;
	PlayerStatus status;
	void addCard(Card* card, bool netCall);
	void removeCard(Card* card, bool netCall);
	bool invalidHand();
	void clearHand();
	void updateStatus();
};

std::string strFromStatus(PlayerStatus status);


