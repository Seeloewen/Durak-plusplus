#include "Player.h"
#include <algorithm>
#include <vector>
#include "Game.h"
#include "NetworkHandler.h"

Player::Player(int id)
{
	this->id = id;
}

void Player::addCard(Card* card, bool netCall)
{
	hand.push_back(card);

	durak->handUi->refresh();

	if (!netCall) sendPacket(ADDCARD, std::format("{};{};{}", std::to_string(id), std::to_string(card->type), card->name));
}

void Player::removeCard(Card* card, bool netCall)
{
	auto it = std::find_if(hand.begin(), hand.end(),
		[&](const Card* k) { return k == card; });
	hand.erase(it);

	durak->handUi->refresh();

	if (!netCall) sendPacket(REMOVECARD, std::format("{};{};{}", std::to_string(id), std::to_string(card->type), card->name));
}

bool Player::invalidHand()
{
	//Count the amount of cards for each type
	int amountSpades = 0, amountHearts = 0, amountDiamonds = 0, amountClubs = 0;

	for (Card* card : hand)
	{
		switch (card->type)
		{
		case SPADES:
			amountSpades++;
			break;
		case CLUBS:
			amountClubs++;
			break;
		case DIAMONDS:
			amountDiamonds++;
			break;
		case HEART:
			amountHearts++;
			break;
		}
	}

	//If player has 5 or more cards of one type, it's "Neumischung"
	return (amountSpades > 4 || amountHearts > 4 || amountDiamonds > 4 || amountClubs > 4);
}

void Player::clearHand()
{
	while (hand.size() > 0)
	{
		removeCard(hand.front(), false);
	}
}