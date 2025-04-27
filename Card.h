#pragma once

#include <string>
#include <qrect.h>
#include <qwidget.h>
#include <qlabel.h>
#include <functional>

enum CardType
{
	SPADES,
	DIAMONDS,
	HEART,
	CLUBS
};

class Card
{
public:
	CardType type;
	int value;
	std::string name;
	std::string id;

	Card(CardType type, int value, std::string name);
	static std::string strFromType(CardType type);
	static std::string nameFromValue(int value);
};

struct CardPair
{
	CardPair(Card* attack);
	CardPair(Card* attack, Card* defense);

	Card* attack = nullptr;
	Card* defense = nullptr;
};

void shuffleCards(std::vector<Card*>* cards);