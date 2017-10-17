#ifndef GOAL_H
#define GOAL_H

#include "Subject.h"
#include <memory>
#include <vector>

using namespace std;

class Player;
class Criterion;

class Goal: public Subject {
	//Private fields of Goal
	const int position;
	bool taken;
	Player *owner;
	vector <Goal*> goalNeighbours;
	vector <Criterion*> criterionNeighbours;
	
	//Private Constructor/Destructor of Goal only available to Board class
	Goal(int pos);
	friend class Board;

 public:
 
	//Overrides Subject's pure virtual method 
	Goal & getSource() override;
	Subject::NotifierType getType() override;
	
	//Accessors to Goal's private fields
	int getPosition() const;	
	bool isTaken() const;
	Player * getOwner() const;

	//Mutator: A Goal achieved by a Player
	bool achievedBy(Player * p);
	bool isAvailable(Player *p);
	
	//Mutator: Add neighbours to Goal
	void addNeighbour(Goal* g);
	void addNeighbour(Criterion* c);
	
	//Load goal data from save
	void loadGoal(Player * p);
	
	//Destructor
	~Goal() = default;
};

#endif
