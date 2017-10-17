#include <iostream>
#include <vector>
#include <cstdlib>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include "Board.h"
#include "Tile.h"
#include "Criterion.h"
#include "Goal.h"
#include "TextDisplay.h"
#include "Dice.h"
#include "Player.h"
using namespace std;

Board::Board() {}

Board::~Board() {}

Board& Board::getSource() {
	return *this;
}

Subject::NotifierType Board::getType() {
	return Subject::NotifierType::BOARD;
}

void Board::clearBoard() {

	td = make_shared<TextDisplay>();
	ld = make_shared<LoadedDice>();
	rd = make_shared<FairDice>();


	// Initiate tiles
	for (int i = 0; i < boardSize; ++i) {
		tiles.push_back(Tile(i));
		tiles[i].attach(const_cast<TextDisplay*>(td.get()));
	}

	// Initiate criterion
	for (int i = 0; i < numCriterion; ++i) {
		criteria.push_back(Criterion(i));
		criteria[i].attach(const_cast<TextDisplay*>(td.get()));
	}

	// Initiate goals
	for (int i = 0; i < numGoals; ++i) {
		goals.push_back(Goal(i));
		goals[i].attach(const_cast<TextDisplay*>(td.get()));
	}
}

randomBoard::randomBoard() {
	init();
}

void randomBoard::init() {
	clearBoard();
	
	// Give random values and resources to every tile

	// Each index represents a resource: 0 = C, 1 = LAB, 2 = LEC, 3 = S, 4 = T, 5 = Netflix
	vector<int> resourceCounter(6, 0);
	// Each index i represents value (i+2) and how many tiles have that value
	vector<int> valueCounter(11, 0);
	
	int temp;
	bool set = false;
	bool gotNetflix = false;
	int tileValue;
	int tileResource;
	for (int i = 0; i < boardSize; ++i) {
		// Get a random number between 2 - 12 for the tile's value
		while(!set) {
			temp = rand() % 11 + 2; // set temp to number between 2 - 12
			switch (temp) {
			case 2:
			case 12:
				if (valueCounter[temp-2] != 1) {
					valueCounter[temp-2] += 1;
					tileValue = temp;
					set = true; 
				}
				break;
			case 7:
				if (valueCounter[temp-2] != 1) {
					valueCounter[temp-2] += 1;
					tileValue = temp;
					gotNetflix = true;
					set = true;
				}
				break;
			default:
				if (valueCounter[temp-2] != 2) {
					valueCounter[temp-2] += 1;
					tileValue = temp;
					set = true;
				}
			}
		}
		set = false;
		if(gotNetflix) {
			tiles[i].setResource(5);
			tiles[i].setValue(tileValue);
			tiles[i].notifyObservers(SubscriptionType::DISPLAY);
			gotNetflix = false;
			continue;
		}
		// Get a random number between 0 - 6 for the tile's resource	
		while(!set) {
			temp = rand() % 6; // set temp to number between 0 - 6
			switch (temp) {
			case 0:
			case 1:
				if (resourceCounter[temp] != 3) {
					resourceCounter[temp] += 1;
					tileResource = temp;
					set = true;
				}
				break;
			case 5:
				break;
			default:
				if (resourceCounter[temp] != 4) {
					resourceCounter[temp] += 1;
					tileResource = temp;
					set = true;
				}
				break;
			}
		}
		set = false;
		tiles[i].setValue(tileValue);
		tiles[i].setResource(tileResource);
		
		tiles[i].notifyObservers(SubscriptionType::DISPLAY);
	}
	
	// ATTACH CORRECT CRITERION TO EACH TILE
	// Create a temporary board;
	vector<vector<Tile>> tempBoard;
	int currentRow = 0;
	int numToAdd = 0;
	int currTile = 0;

	for (int i = 0; i < 9; ++i) { // Add the correct number of tiles per row. Follows the progression: 1, 2, 3, 2, 3, 2, 3, 2, 1
		tempBoard.push_back(vector<Tile>());
		switch (currentRow) {
		case 0:
		case 8:
			numToAdd = 1;
			break;
		case 2:
		case 4:
		case 6:
			numToAdd = 3;
			break;
		default:
			numToAdd = 2;
		}
		for (int j = 0; j < numToAdd; ++j) {
			tempBoard[i].push_back(tiles[currTile]);
			++currTile;
		}
		++currentRow;
	}
	
	
	// Algorithm for attaching the correct Criterion (as observers) to each tile
	int offset1 = 0;
	int offset2 = 0;
	int currentPos = 0;
	int rowSize;
	int currentTile;
	for (int i = 0; i < 9; ++i) {
		switch (i) {
		case(0):
			offset1 = 3;
			offset2 = 5;
			break;
		case(1):
			offset1 = 5;
			offset2 = 6;
			break;
		case(7):
			offset1 = 6;
			offset2 = 5;
			break;
		case(8):
			offset1 = 5;
			offset2 = 3;
			break;
		default:
			offset1 = 6;
			offset2 = 6;
		}
		rowSize = tempBoard[i].size();
		for (int j = 0; j < rowSize; ++j) {
			currentTile = tempBoard[i][j].getPosition();
			tiles[currentTile].attach(&criteria[currentPos]);
			tiles[currentTile].attach(&criteria[currentPos + offset1]);
			tiles[currentTile].attach(&criteria[currentPos + offset1 + offset2]);
			++currentPos;
			tiles[currentTile].attach(&criteria[currentPos]);
			tiles[currentTile].attach(&criteria[currentPos + offset1]);
			tiles[currentTile].attach(&criteria[currentPos + offset1 + offset2]);
			++currentPos;
		}
		if (i > 1) {
			if (i == 7) {
				currentPos += 3;
			}
			else {
				++currentPos;
			}
		}
	}
	initCriteria();
	initGoals();
	// Attach all Tiles to each Dice
	for(int i = 0; i < boardSize; ++i) {
		ld->attach(&tiles[i]);
		rd->attach(&tiles[i]);
	}
	
}

savedBoard::savedBoard() {
	init();
}

void savedBoard::init() { // Board is initiated but Tiles, Criteria, and Goals are 'blank', must call loadBoard and loadOwner (for each player)
	clearBoard();
	
	// ATTACH CORRECT CRITERION TO EACH TILE
	// Create a temporary board;
	vector<vector<Tile>> tempBoard;
	int currentRow = 0;
	int numToAdd = 0;
	int currTile = 0;

	for (int i = 0; i < 9; ++i) { // Add the correct number of tiles per row. Follows the progression: 1, 2, 3, 2, 3, 2, 3, 2, 1
		tempBoard.push_back(vector<Tile>());
		switch (currentRow) {
		case 0:
		case 8:
			numToAdd = 1;
			break;
		case 2:
		case 4:
		case 6:
			numToAdd = 3;
			break;
		default:
			numToAdd = 2;
		}
		for (int j = 0; j < numToAdd; ++j) {
			tempBoard[i].push_back(tiles[currTile]);
			++currTile;
		}
		++currentRow;
	}
	
	// Algorithm for attaching the correct Criterion (as observers) to each tile
	int offset1 = 0;
	int offset2 = 0;
	int currentPos = 0;
    int currentTile;

	for (int i = 0; i < 9; ++i) {
		int rowSize = tempBoard[i].size();
		switch (i) {
		case(0):
			offset1 = 3;
			offset2 = 5;
			break;
		case(1):
			offset1 = 5;
			offset2 = 6;
			break;
		case(7):
			offset1 = 6;
			offset2 = 5;
			break;
		case(8):
			offset1 = 5;
			offset2 = 3;
			break;
		default:
			offset1 = 6;
			offset2 = 6;
		}
		for (int j = 0; j < rowSize; ++j) {
			currentTile = tempBoard[i][j].getPosition();
			tiles[currentTile].attach(&criteria[currentPos]);
			tiles[currentTile].attach(&criteria[currentPos + offset1]);
			tiles[currentTile].attach(&criteria[currentPos + offset1 + offset2]);
			++currentPos;
			tiles[currentTile].attach(&criteria[currentPos]);
			tiles[currentTile].attach(&criteria[currentPos + offset1]);
			tiles[currentTile].attach(&criteria[currentPos + offset1 + offset2]);
			++currentPos;
		}
		if (i > 1) {
			if (i == 7) {
				currentPos += 3;
			}
			else {
				++currentPos;
			}
		}
	}
	initCriteria();
	initGoals();
	// Attach dice
	for(int i = 0; i < boardSize; ++i) {
		ld->attach(&tiles[i]);
		rd->attach(&tiles[i]);
	}
	
}

void Board::initCriteria() { // read from file "cAdjacency.txt"
	ifstream file("cAdjacency.txt");
	int numLines = 54;
	int currLine = 0;
	int i;
	string crit;
	string goal;
	while(currLine < numLines) {
		getline(file, crit, 'G'); // Gets all Criterion positions
		istringstream c {crit};
		while(c >> i) {
			criteria[currLine].addNeighbour(&criteria[i]);
		}
		getline(file, goal); // Gets all Goal positions
		istringstream g {goal};
		while(g >> i) {
			criteria[currLine].addNeighbour(&goals[i]);
		}
		++currLine;
	}
}

void Board::initGoals() { // read from file "gAdjacency.txt"
	 ifstream file("gAdjacency.txt");
	 int numLines = 72;
	 int currLine = 0;
	 int i;
	 string crit;
	 string goal;
	 while(currLine < numLines) {
	 	getline(file, crit, 'G');
	 	istringstream c {crit};
	 	while(c >> i) {
	 		goals[currLine].addNeighbour(&criteria[i]);
	 	}
	 	getline(file, goal);
	 	istringstream g {goal};
	 	while(g >> i) {
	 		goals[currLine].addNeighbour(&goals[i]);
	 	}
	 	++currLine;
	 }
}

bool Board::placeCriterion(Player *p, int pos) {
	if(!criteria[pos].isTaken()) {
		criteria[pos].placedBy(p);
		return true;
	}
	else {
		cout << "You cannot build here." << endl;
		return false;
	}
}

void Board::improveCriterion(Player *p, int pos) {
	bool successfullyCompleted;
	if(criteria[pos].getOwner() != p) {
		cout << "You cannot build here." << endl;
		return;
	}
	int numTut = p->getResource(0);
	int numStudy = p->getResource(1);
	int numCaff = p->getResource(2);
	int numLab = p->getResource(3);
	int numLec = p->getResource(4);
	Criterion::Improvement rank = criteria[pos].getImprovement();
	try {
		switch(rank) {
			case(Criterion::Improvement::Assignment):
				if(numLec >= 2 && numStudy >= 3) {
					successfullyCompleted = criteria[pos].improvedBy(p);
					p->subResource(Resources::LECTURE, 2);
					p->subResource(Resources::STUDY, 3);
					cout << "Student " << p->getName() << " improved Assignment to Midterm." << endl;
				}
				else {
					cout << "You do not have enough resources." << endl;
				}
				break;
			case(Criterion::Improvement::Midterm):
				if(numCaff >= 3 && numLab >= 2 && numLec >= 2 && numTut >= 1 && numStudy >= 2) {
					successfullyCompleted = criteria[pos].improvedBy(p);
					p->subResource(Resources::TUTORIAL, 1);
					p->subResource(Resources::CAFFEINE, 3);
					p->subResource(Resources::LAB, 2);
					p->subResource(Resources::LECTURE, 2);
					p->subResource(Resources::STUDY, 2);
					cout << "Student " << p->getName() << " improved Midterm to Exam." << endl;				}
				else {
					cout << "You do not have enough resources." << endl;
				}
				break;
			default:
				cout << "You cannot build here." << endl;
		}
	}
	catch(char const *error) {
		throw 100;
	}
}

void Board::completeCriterion(Player *p, int pos) {
	bool successfullyCompleted;
	int numTut = p->getResource(0);
	int numCaff = p->getResource(2);
	int numLab = p->getResource(3);
	int numLec = p->getResource(4);
	if(numTut >= 1 && numCaff >= 1 && numLab >= 1 && numLec >= 1) {
		try
		{
			if(!criteria[pos].isTaken()) {
				successfullyCompleted = criteria[pos].improvedBy(p);
				p->subResource(Resources::TUTORIAL, 1);
				p->subResource(Resources::CAFFEINE, 1);
				p->subResource(Resources::LAB, 1);
				p->subResource(Resources::LECTURE, 1);
				cout << "Student " << p->getName() << " completed criterion." << endl;
			}
			if(!successfullyCompleted) {
				cout << "You cannot build here." << endl;
			}
		}
		catch(char const *error) {
			throw 100;
		}
	}
	else {
		cout << "You do not have enough resources." << endl;
	}
}

void Board::achieveGoal(Player *p, int pos) {
	int numTut = p->getResource(0);
	int numStudy = p->getResource(1);
	if(numTut >= 1 && numStudy >= 1) {
		bool successfullyAchieved = goals[pos].achievedBy(p);
		if(successfullyAchieved) {
			p->subResource(Resources::TUTORIAL, 1);
			p->subResource(Resources::STUDY, 1);
			cout << "Student " << p->getName() << " achieved goal." << endl;
		}
		else {
			cout << "You cannot build here." << endl;
		}
	}
	else {
		cout << "You do not have enough resources." << endl;
	}
}

void Board::roll(Player *p) {
	int rollValue;
	try {
		if(p->isLoaded()) {
		// ROLL LOADED DICE
			ld->rollDice();
			rollValue = ld->getRoll();
			switch(rollValue) {
				case(7):
					moveGeese(p);
					break;
				default:
					ld->notifyObservers(SubscriptionType::ROLL);
					if (Tile::noResourcesGiven) cout << "No students gained resources." << endl;
					Tile::noResourcesGiven = true;
			}
		}
		else {
		// ROLL REGULAR DICE
			rd->rollDice();
			rollValue = rd->getRoll();
			cout << "Student " << p->getName() << " rolled " << rollValue << "." << endl;
			switch(rollValue) {
				case(7):
					moveGeese(p);
					notifyObservers(SubscriptionType::GEESE);
					break;
				default:
					rd->notifyObservers(SubscriptionType::ROLL);
					if (Tile::noResourcesGiven) cout << "No students gained resources." << endl;
					Tile::noResourcesGiven = true;
			}
		}
	}
	catch(char const *err) {
		throw;
	}
}

void Board::moveGeese(Player *p) {
	tiles[geesePosition].clearBlocked(); // NEEDS TO BE IMPLEMENTED
	if (tiles[geesePosition].isOverrun() == true) tiles[geesePosition].toggleOverrun();
	this->notifyObservers(SubscriptionType::GEESE);
	cout << "Choose where to place the GEESE" << endl;
	cout << ">";
	int numBlockedPlayers;
	int i;
	string toStealFrom;
	bool stealComplete = false;
	while(true) {
		cin >> i;
		if(cin.eof()) { // EOF
			cin.clear();
			throw("Game over!");
		}
		else if(cin.fail()) { // Not an Integer
			cin.clear();
			cin.ignore(100000000, '\n');
			cout << "Invalid command." << endl;
			cout << ">";
		}
		else if(i < 0 || i > 18) {
			cout << "Invalid command." << endl;
			cout << ">";
		}
		else {
			geesePosition = i;
			if (tiles[i].isOverrun() == false) tiles[i].toggleOverrun();
			// Create list of blocked players
			// MAKE SURE IT'S IN ORDER
			tiles[i].notifyObservers(SubscriptionType::ROLL);
			numBlockedPlayers = tiles[i].blockedPlayers.size();
			if(numBlockedPlayers == 0 || (numBlockedPlayers == 1 && tiles[i].blockedPlayers[0] == p)) {
				cout << "Student " << p->getName() << " has no students to steal from." << endl;
				stealComplete = true;
				return;
			}
			cout << "Student " << p->getName() << " can choose to steal from";
			// avoid same Player name and duplicate Player names
			string print = "";
			for (int h = 0; h < 4; ++h) {
				for(int j = 0; j < numBlockedPlayers; ++j) {
					if (tiles[i].blockedPlayers[j] == p) break;
					if (h == 0 && tiles[i].blockedPlayers[j]->getName() == "BLUE") {
						print += " " + tiles[i].blockedPlayers[j]->getName() + ",";
						break;
					}
					if (h == 1 && tiles[i].blockedPlayers[j]->getName() == "RED") {
						print += " " + tiles[i].blockedPlayers[j]->getName() + ",";
						break;
					}
					if (h == 2 && tiles[i].blockedPlayers[j]->getName() == "ORANGE") {
						print += " " + tiles[i].blockedPlayers[j]->getName() + ",";
						break;
					}
					if (h == 3 && tiles[i].blockedPlayers[j]->getName() == "YELLOW") {
						print += " " + tiles[i].blockedPlayers[j]->getName() + ",";
						break;
					}
				}
				if(h == 3) {
					print = print.substr(0, print.length()-1);
					print  += "." ;
					cout << print << endl;
				}
			}
			break;
		}
	}

	cout << "Choose a student to steal from." << endl;
	cout << ">";
	cin >> toStealFrom;
	// Check for EOF
	if(cin.eof()) {
		cin.clear();
		throw "Game over!";
	}
	while(!stealComplete) {
		for(int j = 0; j < numBlockedPlayers; ++j) {
			if((tiles[i].blockedPlayers[j]->getName() == toStealFrom) && (p->getName() != toStealFrom)) {
				p->steal(tiles[i].blockedPlayers[j]);
				stealComplete = true;
				break;
			}
		}
		if(stealComplete) {
			break;
		}
		cout << "Invalid command." << endl;
		cout << ">";
		cin >> toStealFrom;
		// Check for EOF
		if(cin.eof()) {
		cin.clear();
		throw "Game over!";
		}
	}
}

string Board::exportInfo() const {
	int value;
	int resource;
	Resources r;
	string output = "";
	for(int i = 0; i < boardSize; ++i) {
		value = tiles[i].getValue();
		r = tiles[i].getResource();
		switch(r) {
			case(Resources::CAFFEINE):
				resource = 0;
				break;
			case(Resources::LAB):
				resource = 1;
				break;
			case(Resources::LECTURE):
				resource = 2;
				break;
			case(Resources::STUDY):
				resource = 3;
				break;
			case(Resources::TUTORIAL):
				resource = 4;
				break;
			default:
				resource = 5;
		}
		output += to_string(resource);
		output += " ";
		output += to_string(value);
		output += " ";
	}
	return output;
}

void Board::loadBoard(string info) {
	int currTile = 0;
	int tileValue;
    int resourceValue;
    istringstream ss{info};
    while(ss >> resourceValue) {
        ss >> tileValue;
		tiles[currTile].setValue(tileValue);
		tiles[currTile].setResource(resourceValue);
		tiles[currTile].notifyObservers(SubscriptionType::DISPLAY);
		++currTile;
    }
}

void Board::loadOwner(Player *p, string info) {
	int i;
	int critPos;
	int critRank;
	char c;
	istringstream ss{info};
	ss >> c; //Get rid of first character "g"
	while(true) {
		if(!(ss >> i)) { //When it reaches the next character, "c"
			ss.clear();
			ss.ignore();
			break;
		}
		goals[i].loadGoal(p); //Load info for all the Goals
		goals[i].notifyObservers(SubscriptionType::DISPLAY);
	}
	while(ss >> critPos) { //Will loop through all Criterion
		ss >> critRank;
		criteria[critPos].loadCriterion(p, critRank);
		criteria[critPos].notifyObservers(SubscriptionType::DISPLAY);
	}
}


ostream &operator<<(ostream &out, const Board &b) {
	out << *(b.td);
	return out;
}
