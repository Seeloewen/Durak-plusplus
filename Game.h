#pragma once

#include <vector>
#include "Card.h"
#include "Player.h"
#include "Durak.h"
#include "Attack.h"
#include "string"

const std::string VERSION = "Dev";
const std::string VERDATE = "27.04.2025";

class Game
{
private:
	std::vector<Player*> players;

	void genCards();
	void initialDraw();
	void stockUpCards(Player* p1, Player* p2, Player* p3);

public:
	std::vector<Card*> cardStack;
	Card* selectedCard;
	Attack* currentAttack;
	Player* player;
	CardType trump;
	int playerAmount = 0;

	~Game();
	Player* getPlayer(int id);
	void preInit(int playerAmount, int playerId);
	void postInit();
	void tick();
	void drawCard(Player* player);
	void setCurrentPlayer(int id);
	void setTrump(CardType type);
	int getFinishedPlayers();
	void endGame();
};

void startGame();
void resetGame();

inline Game* game;
inline Durak* durak;
inline QTimer* gameTimer;
inline std::vector<Card*> cardRegister;

Card* getCard(std::string id);