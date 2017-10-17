#include <iostream>
#include "Tile.h"
#include "Dice.h"
#include "Player.h"
#include <iostream>

using namespace std;

// static memeber init
bool Tile::noResourcesGiven = true;


//Constructors of Tile
Tile::Tile(int pos):position{pos}{}
Tile::Tile(int pos,int val,Resources r):position{pos},value{val},resource{r},overrun{false}, blockedPlayers{} {}

//Orerrides Subject's getSource() method
Tile & Tile::getSource() {return *this;}

//Overrides Subject's getType() method
Subject::NotifierType Tile::getType(){return Subject::NotifierType::TILE;}

//Overrides Observer's notify(Subject & notifier) method
void Tile::notify(Subject & diceNotifier){
	//Throws bad_cast if dynamic_cast fails
	Dice & diceRef = dynamic_cast<Dice&>(diceNotifier.getSource());
	if(diceRef.getRoll()==value){
		if(!overrun){
			this->notifyObservers(SubscriptionType::ROLL);
		}
	}
}


//Overrides Observer's subType() method
SubscriptionType Tile::subType() const {return SubscriptionType::ROLL;}

//Accessor to value field
int Tile::getValue() const {return value;}

//Accessor to position field
int Tile::getPosition() const {return position;}

//Accessor to overrrun field
bool Tile::isOverrun() const {return overrun;}

//Accessor to resource field
Resources Tile::getResource() const {return resource;}

//Mutator to overrun field
void Tile::toggleOverrun() {
	overrun = overrun ? false : true;
	this->notifyObservers(SubscriptionType::DISPLAY);
}

//Mutator to resource field
void Tile::setResource(int r){resource = static_cast<Resources>(r);}

//Mutator to value field
void Tile::setValue(int v) {value = v;}

//Mutator to blockedPlayers field
void Tile::addBlocked(Player *player) {
	if (player->getTotalResource() == 0) return;
	int size = blockedPlayers.size();
	for (int i = 0; i < size; ++i) {
		if (blockedPlayers[i] == player) return;
	}
	blockedPlayers.emplace_back(player);
}

void Tile::clearBlocked() {
	blockedPlayers.clear();
}
