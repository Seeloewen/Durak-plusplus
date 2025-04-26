#pragma once

#include <vector>
#include "Card.h"
#include "Player.h"
#include "Durak.h"
#include "Attack.h"
#include "string"

const std::string VERSION = "Dev";
const std::string VERDATE = "20.04.2025";

class Game
{
public:
	std::vector<Player*> players;
	std::vector<Card*> cardStack;
	Card* selectedCard;
	Attack* currentAttack;
	CardType trump;
	Player* player;
	int playerAmount = 0;
	bool initialized;

	~Game();
	Player* getPlayer(int id);
	Card* getCard(CardType type, std::string name);
	void preInit(int playerAmount, int playerId);
	void postInit();
	void initialDraw();
	void genCards();
	void tick();
	void setPlayer(int id);
	void drawCard(Player* player);
	void stockUpCards(Player* p1, Player* p2, Player* p3);
	void setTrump(CardType type);
};

inline Game* game;
inline Durak* durak;
inline QTimer* gameTimer;
inline std::vector<Card*> cardRegister;