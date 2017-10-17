#include "Board.h"
#include "TextDisplay.h"
#include "Controller.h"
#include <iostream>
#include <string>
#include <ctime>
#include <random>

using namespace std;

int main(int argc, char *argv[]) {
	int seed;
	int howToLoad = 0;
	string fileName;
	bool seeded = false;
	Controller gameController;
	// Check if command-line arguments were used, assuming each argument is used correctly
	if(argc > 1) {
		for(int i = 1; i < argc; i+=2) {
			if(string(argv[i]) == "-seed") {
				seed = stoi(argv[i+1]);
				seeded = true;
			}
			if(string(argv[i]) == "-load") {
				howToLoad = 1;
				fileName = argv[i+1];
			}
			if(string(argv[i]) == "-board") {
				howToLoad = 2;
				fileName = argv[i+1];
			}
		}
	}
	// Set random seed if option exists
	if(seeded) {
		srand(seed);
	}
	else { // Else set same fixed seed
		srand(69);
	}
	
	switch(howToLoad) {
		case(0):
			gameController.startGame();
			
			break;
		case(1):
			gameController.loadGameFromFile(fileName);
			break;
		default:
			gameController.loadBoardFromFile(fileName);
	}
	startingPoint:	
	try {
		if (howToLoad != 1) {
			gameController.getSetupMove(cin, cout);
		}
		while(true) {
			gameController.getBOTmove(cin, cout);
			gameController.getEOTmove(cin, cout);
		}
	}
	catch(char const *err) {
		return 0;
	}
	catch(int won) {
		cout << "Would you like to play again?" << endl;
		cout << ">";
		string answer;
		while(cin >> answer) {
			if(answer == "yes") {
				gameController.restartGame();
				howToLoad = 0;
				goto startingPoint;
			}
			else if(answer == "no") {
				return 0;
			} 
			else {
				cout << "Invalid command." << endl;
				cout << ">";
			}
		}
		return 0;
	}
}
