#include "Card.h"
#include <qlabel.h>
#include "Game.h"
#include <qwidget.h>
#include <qpainter.h>
#include <qstring.h>
#include <algorithm>
#include <random>
#include <qobject.h>

//-- Card --//

Card::Card(CardType type, int value, std::string name)
{
	this->type = type;
	this->value = value;
	this->name = name;

	id = std::format("{}_{}", strFromType(type), name);
}

std::string Card::strFromType(CardType type)
{
	//Get the string from the type
	switch (type)
	{
	case SPADES:
		return "Spades";
	case DIAMONDS:
		return "Diamonds";
	case HEART:
		return "Hearts";
	case CLUBS:
		return "Clubs";
	default:
		return "Unknown";
	}
}

std::string Card::nameFromValue(int value)
{
	//Get the name from the value
	switch (value)
	{
	case 11:
		return "Jack";
	case 12:
		return "Queen";
	case 13:
		return "King";
	case 14:
		return "Ace";
	default:
		return std::to_string(value);
	}
}

void shuffleCards(std::vector<Card*>* cards)
{
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::ranges::shuffle(*cards, rng);
}

//-- CardPair --//

CardPair::CardPair(Card* attack)
{
	this->attack = attack;
}

CardPair::CardPair(Card* attack, Card* defense)
{
	this->attack = attack;
	this->defense = defense;
}