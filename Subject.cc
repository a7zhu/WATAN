#include "Subject.h"
#include "Observer.h"

void Subject::attach(Observer *o){
	observers.emplace_back(o);
}

void Subject::notifyObservers(SubscriptionType t){
	for(auto ob : observers){
		if(ob->subType()==t) ob->notify(*this);
	}
}

int Subject::numObservers() {
	return observers.size();
}

Subject::~Subject(){}
