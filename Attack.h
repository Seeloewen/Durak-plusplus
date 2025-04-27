#pragma once

#include "Player.h"
#include "list"
#include "Card.h"
#include "QWidget"

class Attack
{
private:
	Attack(Player* def, Player* att1, Player* att2);
	bool karteLiegt(int value);
	int getUndefendedAmount();

public:
	std::vector<CardPair*> cardPairs;
	std::vector<Player*> quitPlayers;
	bool isFinished = false;
	bool isDefended = false;

	Player* defender;
	Player* attacker1;
	Player* attacker2;

	~Attack();
	static void createAttack(Player* def, Player* att1, Player* att2, bool netCall);
	void addCard(Player* attacker, Card* card, bool netCall);
	void defend(Card* attack, Card* defense, bool netCall);
	void leave(Player* player, bool netCall);
	void finish();
};

