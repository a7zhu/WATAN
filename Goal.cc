#include "Goal.h"
#include "Player.h"
#include "Criterion.h"

using namespace std;

//Constructor of Goal
Goal::Goal(int pos):position{pos},taken{false},owner{nullptr}{}

//Overrides Subject's getSource() method
Goal & Goal::getSource(){return *this;}

//Overrides Subject's getType() method
Subject::NotifierType Goal::getType() {return Subject::NotifierType::GOAL;} 

//Accessor to position field
int Goal::getPosition() const {return position;}

//Accessor to taken field
bool Goal::isTaken() const {return taken;}

//Accessor to owner fiekd
Player * Goal::getOwner() const {return owner;}

//Goal achieved by a Player
bool Goal::achievedBy(Player * p){
	if(isAvailable(p)) {
		taken = true;
		owner = p;
		p->addGoal(position);
		this->notifyObservers(SubscriptionType::DISPLAY);
		return true;
	}
	else {
		return false;
	}
}

//Adds neighbours to neighbours feilds
bool Goal::isAvailable(Player *p) {
	int numCrit = criterionNeighbours.size();
	int numGoals = goalNeighbours.size();
	Player *temp;
	if(!taken) {
		for(int j = 0; j < numGoals; ++j) {
			if(goalNeighbours[j]->isTaken() && goalNeighbours[j]->getOwner() == p) {
				return true;
			}
		}
		for(int i = 0; i < numCrit; ++i) {
			temp = criterionNeighbours[i]->getOwner();
			if(temp == p) {
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}
}

void Goal::addNeighbour(Criterion* c) {
	this->criterionNeighbours.emplace_back(c);
}

void Goal::addNeighbour(Goal* g) {
	this->goalNeighbours.emplace_back(g);
}

//load Goal data from save
void Goal::loadGoal(Player * p) {
	taken = true;
	owner = p;
}
