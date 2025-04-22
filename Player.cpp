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

	durak->handUi->update();

	if(!netCall) sendPacket(ADDCARD, std::format("{};{};{}", std::to_string(id), std::to_string(card->type), card->name));
}

void Player::removeCard(Card* card, bool netCall)
{
	auto it = std::find_if(hand.begin(), hand.end(),
		[&](const Card* k) { return k == card; });
	hand.erase(it);

	durak->handUi->update();

	if (!netCall) sendPacket(REMOVECARD, std::format("{};{};{}", std::to_string(id), std::to_string(card->type), card->name));
}