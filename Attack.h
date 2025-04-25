#pragma once

#include "Player.h"
#include "list"
#include "Card.h"
#include "QWidget"

class Attack
{
public:
	std::vector<CardPair*> cardPairs;
	bool isFinished = false;
	bool isDefended = false;
	int defId = 0;
	int att1Id = 0;
	int att2Id = 0;

	Player* defender;
	Player* attacker1;
	Player* attacker2;

	Attack(Player* defender, Player* attacker1, Player* attacker2, bool netCall);
	~Attack();
	void addCard(Player* attacker, Card* card, bool netCall);
	void defend(Card* attack, Card* defense, bool netCall);
	void leave(Player* player, bool netCall);
	bool karteLiegt(int value);
};