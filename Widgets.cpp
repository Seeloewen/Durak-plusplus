#include "Widgets.h"
#include "Game.h"
#include "TextureManager.h"

//-- HandUi --//

HandUi::HandUi(QWidget* parent) : QWidget(parent)
{
	setGeometry(20, durak->height() - 200, durak->width() - 40, 180);
	setStyleSheet("background-color: rgb(100, 200, 300);");
}

void HandUi::setPlayer(Player* player)
{
	this->player = player;
}

void HandUi::refresh()
{
	//Clear previous hand
	for (CardUi* cardUi : cards)
	{
		delete cardUi;
	}
	cards.clear();


	int amountCards = player->hand.size();
	int space = 22;
	int prev = space;

	//What even is this :skull:
	if (amountCards > 10) space = 0.0552083333 * amountCards * amountCards * amountCards - 2.76875 * amountCards * amountCards + 41.1791667 * amountCards - 202.125;

	for (Card* card : player->hand)
	{
		int x = prev;
		CardUi* cardUi = new CardUi(x, 20, this);
		cardUi->setCard(card);
		cards.push_back(cardUi);

		cardUi->onClick = [this, card]() {
			if (game->currentAttack->defender == player)
			{
				game->selectedCard = card;
			}
			else if (game->currentAttack->attacker1 == player
				|| game->currentAttack->attacker2 == player)
			{
				game->currentAttack->addCard(player, card, false);
			}
			};

		prev = prev + space + 100;
	}

	update();
}

//-- AttackUi --//

AttackUi::AttackUi(QWidget* parent) :QWidget(parent)
{
	setGeometry(20, 20, 740, 220);
	setStyleSheet("background-color: rgb(100, 200, 300);");
}

void AttackUi::refresh()
{
	for (CardUi* cardUi : cardUis)
	{
		delete cardUi;
	}
	cardUis.clear();

	int i = 0;
	for (CardPair* pair : game->currentAttack->cardPairs)
	{
		addAttack(pair->attack, i);
		if (pair->defense != nullptr)
		{
			addDefense(pair->defense, i);
		}
		i++;
	}

	update();
}

void AttackUi::addAttack(Card* card, int index)
{
	int x = (index + 1) * 20 + index * 100;
	int y = 20;
	CardUi* cardUi = new CardUi(x, y, this);
	cardUi->setCard(card);
	cardUi->onClick = [card]()
		{
			if (game->selectedCard != nullptr)
			{
				game->currentAttack->defend(card, game->selectedCard, false);
				game->selectedCard = nullptr;
			}
		};

	cardUis.push_back(cardUi);
}

void AttackUi::addDefense(Card* card, int index)
{
	CardUi* cardUi = new CardUi((index + 1) * 20 + index * 100, 60, this);
	cardUi->setCard(card);
	cardUis.push_back(cardUi);
}

//-- CardUi --//

CardUi::CardUi(int x, int y, QWidget* parent) : QWidget(parent)
{
	setGeometry(x, y, 100, 140);

	image = new QLabel(this);
	image->resize(100, 140);

	show();
}

CardUi::~CardUi()
{
	hide();
}

void CardUi::mousePressEvent(QMouseEvent* event)
{
	onClick();
}

void CardUi::setCard(Card* card)
{
	this->card = card;

	QPixmap pixmap = QPixmap::fromImage(*getTexture(card->id));
	QPixmap scaledPixmap = pixmap.scaled(image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	image->setPixmap(scaledPixmap);

	update();
}