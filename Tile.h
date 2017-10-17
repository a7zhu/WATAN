#ifndef TILE_H
#define TILE_H

#include "Subject.h"
#include "Observer.h"
#include "Resources.h"
#include <vector>

class Player;

class Tile : public Subject, public Observer {
	
	//Private fields of Tile
	int position;
	int value;
	Resources resource;
	bool overrun = false;
	std::vector<Player*> blockedPlayers;
	

	//Private Constructors of Tile only available to Board class
	Tile(int pos);
	Tile(int pos, int val, Resources r);
	friend class Board;

 public:
 
	// if it gives resources
	static bool noResourcesGiven;
	
	//Overrides Subject's pure virtual method
	Tile & getSource() override;
	Subject::NotifierType getType() override;

	//Overrides Observer's pure virtual method		
	void notify(Subject & diceSubject) override;
	SubscriptionType subType() const override;

	//Accessors to Tile's fields
	int getValue() const;
	int getPosition() const;
	bool isOverrun() const;
	Resources getResource() const;

	//Mutators to Tile's fields
	//Geese overrun or withdraw from a Tile
	void toggleOverrun();
	void setResource(int r);
	void setValue(int v);
	void addBlocked(Player *p);
	void clearBlocked();
	static void resourcesGiven(){noResourcesGiven = false;}
	
	//Destructor
	~Tile()=default;
};
#endif
