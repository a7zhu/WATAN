#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include "Player.h"
#include "Resources.h"

class Board;

class Controller{
	std::shared_ptr<Board> board;
	int turnNum = 0;
	std::vector<Player> players;
	const int BLUE = 0;
	const int RED =1;
	const int ORANGE = 2;
	const int YELLOW = 3;
	
	int readPosition(std::istream & in, std::ostream & out, int lo, int hi, bool arrow=true);
	
	//private helper function that saves the game and throws an exception when EOF
	void saveGame();
		
  public:
	Controller();
	
	void startGame();

	void loadBoardFromFile(std::string filename);

	void loadGameFromFile(std::string filename);
	
	// setting up the game command
	void getSetupMove(std::istream & in, std::ostream & out );
	
	// beginning of the turn command
	void getBOTmove(std::istream & in, std::ostream & out);
	
	// end of turn command
	void getEOTmove(std::istream & in, std::ostream & out);
	
	void restartGame();
};

#endif
