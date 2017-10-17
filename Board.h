#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include "Tile.h"
#include "Criterion.h"
#include "Goal.h"
#include <memory>

class Dice;
class TextDisplay;

class Board: public Subject{
	protected:
		int boardSize = 19;
		int numCriterion = 54;
		int numGoals = 72; // numCriterion + (boardSize - 1)
		std::vector<Tile> tiles;
		std::vector<Criterion> criteria;
		std::vector<Goal> goals;
		std::shared_ptr<TextDisplay> td;
		std::shared_ptr<Dice> ld;
		std::shared_ptr<Dice>rd;
		int geesePosition;
		
		
	public:
		Board();
		~Board();
			
		//Overrides Subject's pure virtual methods
		Board& getSource() override;
		Subject::NotifierType getType() override;
		
		//Gameplay methods
		bool placeCriterion(Player *p, int pos);
		void improveCriterion(Player *p, int pos);
		void completeCriterion(Player *p, int pos);		
		void achieveGoal(Player *p, int pos);
		void roll(Player * p);
		void moveGeese(Player *p);
		
		//Methods for loading, exporting the board
		std::string exportInfo() const;
		void loadBoard(std::string info);
		void loadOwner(Player *p, std::string info);
		
		virtual void init() = 0; // initiates the board
		void initCriteria();
		void initGoals();
		void clearBoard();
		friend std::ostream &operator<<(std::ostream &out, const Board &b);
};

class randomBoard: public Board {
	public:
		randomBoard();
		void init() override; 
};

class savedBoard: public Board {
	public:
		savedBoard();
		void init() override;
};
#endif
