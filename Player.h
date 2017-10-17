#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include "Resources.h"
#include "Observer.h"
class Criterion;
class Goal;


class Player : public Observer {
	private:
		std::vector<int> numResources;
		bool loaded;
		int victoryPoints;
		int totalResources;
		std::string color;
		std::vector<int> ownedGoals;
		std::vector<int> ownedCriteria;
		std::vector<int> criteriaCompletion; // stores the completion of each criterion at the same position as ownedCriteria
	public:
		Player(std::string c);
		
		// gets the info of player
		std::string exportInfo() const;
		std::string printStatus() const;
		std::string getName() const;
		int getResource(const int n) const;
		bool isLoaded() const;
		int getTotalResource() const;
		
		// print criteria info
		std::string printCriteria() const;
		
		// trade command
		void trade(Player *p, Resources r1, Resources r2);
		
		// change resources after a roll
		void addResource(Resources r, const int n);
		void subResource(Resources r, const int n);
		
		// set dice to loaded or fair
		void setDiceToLoad();
		void setDiceToFair();
		
		// add / find Goals and Criteria
		void addGoal(const int pos);
		void addCriterion(const int pos);
		void updateCriteria(const int pos); // updates the completion in criteriaCompletion at positio pos
		int findPosCriterion(const int val) const; // finds the position of val in ownedCriteria
		
		// load stats from save
		void loadStats(std::string f);
		
		// if rolled 7
		void cut(int n); //take in argument int n, to improve generality
		void steal(Player* p);
		
		// check if a player has won
		bool win() const; // if 10 VP, game ends
		
		// override Obserserver functions
		void notify(Subject& notifier) override; // calls cut
		SubscriptionType subType() const  override;
};

#endif
