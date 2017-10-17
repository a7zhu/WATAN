#ifndef _SUBJECT_H
#define _SUBJECT_H

#include <vector>
#include "Subscriptions.h"
class Observer;

class Subject {
    std::vector<Observer*> observers;
  public:
	enum NotifierType {CRITERION, GOAL, TILE, BOARD, DICE};
    void attach(Observer *o);
    void notifyObservers(SubscriptionType t);
	int numObservers();
	virtual Subject::NotifierType getType() = 0;
    virtual Subject& getSource() = 0;
    virtual ~Subject() = 0;
};

#endif
