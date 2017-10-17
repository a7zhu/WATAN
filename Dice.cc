#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include "Dice.h"
using namespace std;


Dice & Dice::getSource(){return *this;}

Subject::NotifierType Dice::getType(){return Subject::NotifierType::DICE;} 

void LoadedDice::rollDice() {
	int x;
	cout << "Input a roll between 2 and 12:" << endl;
	cout << ">";
	while (true) {
		cin >> x;
		if (cin.eof()) {
			cin.clear();
			throw "Game over!"; // catch in Controller class to save game and end program.
		}
		if (cin.fail()) {
			cout << "Invalid roll." << endl;
			cout << "Input a roll between 2 and 12:" << endl;
			cout << ">";
			cin.clear();
			cin.ignore(100000, '\n');
			continue;
		} else if (x < 2 || x > 12) {
			cout << "Invalid roll." << endl;
			cout << "Input a roll between 2 and 12:" << endl;
			cout << ">";
			continue;
		} else {
			break;
		}
	}
	currentRoll = x;
}

void FairDice::rollDice() {
	int firstDie;
	int secondDie;

	firstDie = rand() % 6 + 1;
	secondDie = rand() % 6 + 1;
	currentRoll = firstDie + secondDie;
}

int Dice::getRoll() const {return currentRoll;}
