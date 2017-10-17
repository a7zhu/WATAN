#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include "Player.h"
#include "Criterion.h"
#include "Goal.h"
#include "Observer.h"
#include "Resources.h"
#include "Subscriptions.h"

using namespace std;

// constructor
Player::Player(string c) : numResources(5, 0), loaded{false}, victoryPoints{0}, 
	totalResources{0}, color{c}, ownedGoals{}, ownedCriteria{}, criteriaCompletion{} {}

// gives string of status
string Player::printStatus() const {
	string status = "";
	status += color;
	status += " has ";
	status += to_string(victoryPoints);
	status += " victory points, ";
	status += to_string(numResources[2]);
	status += " caffeines, ";
	status += to_string(numResources[3]);
	status += " labs, ";
	status += to_string(numResources[4]);
	status += " lectures, ";
	status += to_string(numResources[0]);
	status += " tutorials, and ";
	status += to_string(numResources[1]);
	status += " studies.";
	return status;
}

// gives the save game equivalent of info
string Player::exportInfo() const {
	string info = "";
	info += to_string(numResources[2]);
	info += " ";
	info += to_string(numResources[3]);
	info += " ";
	info += to_string(numResources[4]);
	info += " ";
	info += to_string(numResources[1]);
	info += " ";
	info += to_string(numResources[0]);
	info += " g";
	int size = ownedGoals.size();
	for (int i = 0; i < size; ++i) {
		info += " ";
		info += to_string(ownedGoals[i]);
	}
	info += " c";
	size = ownedCriteria.size();
	for (int i = 0; i < size; ++i) {
		info += " ";
		info += to_string(ownedCriteria[i]);
		info += " ";
		info += to_string(criteriaCompletion[i]);
	}
	return info;
}

// return Player color
string Player::getName() const {return color;}

// return player resource
int Player::getResource(const int n) const {return numResources[n];}

int Player::getTotalResource() const {return totalResources;}

// return if player has loaded dice
bool Player::isLoaded() const {return loaded;}

// print criteria
string Player::printCriteria() const {
	string status = "";
	status += color;
	status += " has completed:";
	int size = ownedCriteria.size();
	for (int i = 0; i < size; ++i) {
		status += "\n";
		status += to_string(ownedCriteria[i]);
		status += " ";
		status += to_string(criteriaCompletion[i]);
	}
	return status;
}

// helper to convery enum to string and int
static string resourceLookup(Resources r){
	if(r==Resources::CAFFEINE) return "CAFFEINE";
	else if(r==Resources::LAB) return "LAB";
	else if(r==Resources::LECTURE) return "LECTURE";
	else if(r==Resources::STUDY) return "STUDY";
	else return "TUTORIAL";
}

static int resourceFind(Resources r){
	if(r==Resources::CAFFEINE) return 2;
	else if(r==Resources::LAB) return 3;
	else if(r==Resources::LECTURE) return 4;
	else if(r==Resources::STUDY) return 1;
	else return 0;
}

// trade function
void Player::trade(Player *p, Resources r1, Resources r2) {
	int res1 = resourceFind(r1);
	int res2 = resourceFind(r2);
	if (numResources[res1] == 0 || p->getResource(res2) == 0) {
		cout << "You do not have enough resources" << endl;
		return;
	}
	string cmd = "";
	cmd += color;
	cmd += " offers ";
	cmd += p->getName();
	cmd += " one ";
	cmd += resourceLookup(r1);
	cmd += " for one ";
	cmd += resourceLookup(r2);
	cmd += ".";
	cout << cmd << endl << "Does " << p->getName() << " accept this offer?" << endl;
	cout << ">";
	string answer;
	while (cin >> answer) {
		if (cin.eof()) {
			cin.clear();
			throw "Game over!"; // catch exception in Controller class and save game and end program.
		}
		if (answer == "no") {
			return;
		} else if (answer == "yes") {
			break;
		} else {
			cout << "Invalid input" << endl;
			cout << ">";
			continue;
		}
	}
	--numResources[res1];
	++numResources[res2];
	p->addResource(r1, 1);
	p->subResource(r2, 1);
}

// add and subtract resources
void Player::addResource(Resources r, const int n) {
	int res = resourceFind(r);
	numResources[res] += n;
	totalResources += n;
}

void Player::subResource(Resources r, const int n) {
	int res = resourceFind(r);
	numResources[res] -= n;
	totalResources -= n;
}

// change state of dice
void Player::setDiceToLoad() {
	loaded = true;
}

void Player::setDiceToFair() {
	loaded = false;
}

// add / find criteria and goals of player
void Player::addGoal(const int pos) {
	ownedGoals.emplace_back(pos);
}

void Player::addCriterion(const int pos) {
	ownedCriteria.emplace_back(pos);
	criteriaCompletion.emplace_back(1);
	++victoryPoints;
}

void Player::updateCriteria(const int pos) {
	++criteriaCompletion[pos];
	++victoryPoints;
}

int Player::findPosCriterion(const int val) const {
	int pos = 0;
	int size = ownedCriteria.size();
	for (int i = 0; i < size; ++i) {
		pos = i;
		if (ownedCriteria[i] == val) break;
	}
	return pos;
}

// load stats for player from save
void Player::loadStats(string f) {
	totalResources = 0;
	istringstream ss(f);
	string word;
	int input;
	int num;
	ss >> input;
	numResources[2] = input;
	totalResources += input;
	ss >> input;
	numResources[3] = input;
	totalResources += input;
	ss >> input;
	numResources[4] = input;
	totalResources += input;
	ss >> input;
	numResources[1] = input;
	totalResources += input;
	ss >> input;
	numResources[0] = input;
	totalResources += input;
	ss >> word;
	while (ss >> word) {
		if (word == "c") break;
		num = stoi(word);
		ownedGoals.emplace_back(num);
	}
	while (ss >> word) {
		num = stoi(word);
		ownedCriteria.emplace_back(num);
		ss >> word;
		num = stoi(word);
		criteriaCompletion.emplace_back(num);
		victoryPoints += num;
	}
}

// cut player when 7 rolled
void Player::cut(int n) {
	if (totalResources >= n) {
		int times =  totalResources / 2;
		int tut;
		int study;
		int caf;
		int lab;
		int val;
		int res;
		int numTut = 0;
		int numStudy = 0;
		int numCaf = 0;
		int numLab = 0;
		int numLec = 0;
		for (int i = 0; i < times; ++i) {
			tut = numResources[0];
			study = numResources[1];
			caf = numResources[2];
			lab = numResources[3];
			val = (rand() % totalResources + 1);
			if (val < tut && tut != 0) {
				res = 0;
			} else if ((val < (tut + study)) && study != 0 ) {
				res = 1;
			} else if ((val < (tut + study + caf)) && caf != 0) {
				res = 2;
			} else if ((val < (tut + study + caf + lab)) && lab != 0) {
				res = 3;
			} else {
				res = 4;
			}
			if (res == 0) {
				++numTut;				
			} else if (res == 1) {
				++numStudy;
			} else if (res == 2) {
				++numCaf;
			} else if (res == 3) {
				++numLab;
			} else {
				++numLec;
			}
			--numResources[res];
			--totalResources;
		}
		cout << "Student " << color << " loses " << times << " resources to the geese. They lose:" << endl;
		if (numCaf != 0) cout << numCaf << " CAFFEINE" << endl;
		if (numLab != 0) cout << numLab << " LAB" << endl;
		if (numLec != 0) cout << numLec << " LECTURE" << endl;
		if (numTut != 0) cout << numTut << " TUTORIAL" << endl;
		if (numStudy != 0) cout << numStudy << " STUDY" << endl;
	}
}

// steal from another player
void Player::steal(Player* p) {
	int tut;
	int study;
	int caf;
	int lab;
	int val;
	int res;
	tut = numResources[0];
	study = numResources[1];
	caf = numResources[2];
	lab = numResources[3];
	val = (rand() % totalResources + 1);
	if (val < tut && tut != 0) {
		res = 0;
	} else if ((val < tut + study) && study != 0) {
		res = 1;
	} else if ((val < tut + study + caf) && caf != 0) {
		res = 2;
	} else if ((val < tut + study + caf + lab) && lab != 0) {
		res = 3;
	} else {
		res = 4;
	}
	++numResources[res];
	++totalResources;
	Resources r;
	cout << "Student " << color << " steals";
	if(res==2) {
		r = Resources::CAFFEINE;
		cout << " CAFFEINE from student " << p->getName() << "." << endl;
	} else if(res==3) {
		r = Resources::LAB;
		cout << " LAB from student " << p->getName() << "." << endl;
	} else if(res==4) {
		r = Resources::LECTURE;
		cout << " LECTURE from student " << p->getName() << "." << endl;
	} else if(res==1) {
		r = Resources::STUDY;
		cout << " STUDY from student " << p->getName() << "." << endl;
	} else {
		r = Resources::TUTORIAL;
		cout << " TUTORIAL from student " << p->getName() << "." << endl;
	}
	p->subResource(r, 1);
}

// does player win?
bool Player::win() const {
	if (victoryPoints >= 10) {
		return true;
	} else {
		return false;
	}
}

// observer overrides
void Player::notify(Subject& notifier) {
	cut(10);
}

SubscriptionType Player::subType() const {
	return SubscriptionType::GEESE;
}
