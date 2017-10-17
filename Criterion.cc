#include "Criterion.h"
#include "Player.h"
#include "Tile.h"
#include "Goal.h"
#include "Resources.h"

#include <iostream>

using namespace std;

//Criterion Constructor
Criterion::Criterion(int pos): position{pos},taken{false},owner{nullptr},improvement{Criterion::Improvement::None}{};

//Overrides Subject's getSource() method 
Criterion & Criterion::getSource() {return *this;}

//Overrides Subject's getType() method
Subject::NotifierType Criterion::getType() {return Subject::NotifierType::CRITERION;}



static string lookup(Resources r){
	if(r==Resources::CAFFEINE) return "CAFFEINE";
	else if(r==Resources::TUTORIAL) return "TUTORIAL";
	else if(r==Resources::LAB) return "LAB";
	else if(r==Resources::STUDY) return "STUDY";
	else return "LECTURE";

}

//Overrides Observer's notify(Subject & whoNotified) method
void Criterion::notify(Subject & tileNotifier){
	Tile & ref = dynamic_cast<Tile&>(tileNotifier.getSource());
	bool tileOverrun = ref.isOverrun();
	if(owner && !tileOverrun){
		int howMany = 1;
		if(improvement==Criterion::Improvement::Midterm) howMany=2;
		else if(improvement==Criterion::Improvement::Exam) howMany=3;
		//Throws bad_cast if dynamic_cast fails
		Tile & tileRef = dynamic_cast<Tile&>(tileNotifier.getSource());	
		owner->addResource(tileRef.getResource(), howMany);
		Tile::resourcesGiven();
		cout << "Student " << owner->getName() << " gained:" << endl;
		cout << howMany<< " "<<lookup(tileRef.getResource())<<endl;
	}
	else if(owner && tileOverrun) {
		Tile & reff = dynamic_cast<Tile&>(tileNotifier.getSource());
		reff.addBlocked(owner);
	}
}

//Overrides Observer's subType() method
SubscriptionType Criterion::subType() const{
	return SubscriptionType::ROLL;
}

//Accessor to taken field
bool Criterion::isTaken() const {return taken;}

//Accessor to position field
int Criterion::getPosition() const {return position;}

//Accessor to owner field
Player * Criterion::getOwner() const {return owner;}

//Accessor to improvement field
Criterion::Improvement Criterion::getImprovement() const {return improvement;}

//Improves Criterion from Assignment->Midterm->Exam
bool Criterion::improvedBy(Player * p){
	if(isAvailable(p)){
		changeTaken();
		owner = p;
		improvement = Criterion::Improvement::Assignment;
		p->addCriterion(position);
		if (p->win()) throw "Game over!";
		this->notifyObservers(SubscriptionType::DISPLAY);
		notifyNeighbours();
		return true;
	}else if(p==owner){
		if(improvement==Criterion::Improvement::Assignment){
			improvement = Criterion::Improvement::Midterm;
			int place = p->findPosCriterion(position);
			p->updateCriteria(place);
			if (p->win()) throw "Game over!";
		}else if(improvement==Criterion::Improvement::Midterm){
			improvement = Criterion::Improvement::Exam;
			int place = p->findPosCriterion(position);
			p->updateCriteria(place);
			if (p->win()) throw "Game over!";
		}else return false;  
		//Criterion reached Exam; Cannot be improved anymore
		this->notifyObservers(SubscriptionType::DISPLAY);
		return true;
	}else return false; //owner!=p
}

bool Criterion::placedBy(Player *p) {
	if(!isTaken()) {
		changeTaken();
		owner = p;
		improvement = Criterion::Improvement::Assignment;
		p->addCriterion(position);
		notifyObservers(SubscriptionType::DISPLAY);
		notifyNeighbours();
		return true;
	}
	else {
		return false;
	}
}

//Adds neighbours to neighbours fields
void Criterion::changeTaken() {
	taken = true;
}

bool Criterion::isAvailable(Player *p) {
	int numGoals = goalNeighbours.size();
	if(!taken) {
		for(int i = 0; i < numGoals; ++i) {
			if(goalNeighbours[i]->isTaken() && goalNeighbours[i]->getOwner() == p) {
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}
}
void Criterion::addNeighbour(Criterion *c) {
	this->criterionNeighbours.emplace_back(c);
}

void Criterion::addNeighbour(Goal *g) {
	this->goalNeighbours.emplace_back(g);
}

void Criterion::notifyNeighbours() {
	int numCrit = criterionNeighbours.size();
	// Make all neighbouring Criterion unavailable
	for(int i = 0; i < numCrit; ++i) {
		criterionNeighbours[i]->changeTaken();
	}
}

// Load function to update Criterion info
void Criterion::loadCriterion(Player * p, int n) {
	taken = true;
	owner = p;
	if (n == 1) {
		improvement = Criterion::Improvement::Assignment;
	} else if (n == 2) {
		improvement = Criterion::Improvement::Midterm;
	} else {
		improvement = Criterion::Improvement::Exam;
	}
	notifyNeighbours();
}
