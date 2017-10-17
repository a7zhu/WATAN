#ifndef CRITERION_H
#define CRITERION_H

#include "Subject.h"
#include "Observer.h"
#include <vector>


class Player;
class Goal;

class Criterion : public Subject, public Observer {
 public: 
	// Improvement types
	enum Improvement{
		None,
		Assignment,
		Midterm,
		Exam
	};

	//Overrides Subject's pure virtual method
	Criterion & getSource() override;
	Subject::NotifierType getType() override;

	//Overrides Observer's  pure virtual methods
	void notify(Subject & tileNotifier) override;
	SubscriptionType subType() const  override;

	//Accessors to Criterion's private fields
	bool isTaken() const;
	int getPosition() const;
	Player * getOwner() const;
	Improvement getImprovement() const;
		
	//Mutator: A Criterion improved by a Player
	//Assignment->Midterm->Exam
	bool improvedBy(Player * p);
	bool placedBy(Player *p);
	
	//Mutator: Add neighbours to Criterion
	void addNeighbour(Goal* g);
	void addNeighbour(Criterion* c);
	void notifyNeighbours();
	
	void changeTaken();
	bool isAvailable(Player * p);
	
	// Called when loading
	void loadCriterion(Player * p, int n);
	
	//Destructor
	~Criterion() = default;
	
 private:
 
	//Fields of Criterion;
	const int position;
	bool taken;
	Player * owner;
	Improvement improvement;
	std::vector <Goal*> goalNeighbours;
	std::vector <Criterion*> criterionNeighbours;

	//Private constructor of Criterion only available to Board class
	Criterion(int pos);
	friend class Board;
};

#endif
