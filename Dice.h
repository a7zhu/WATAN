#ifndef DICE_H
#define DICE_H

#include "Subject.h"

class Dice: public Subject {
	protected:
		int currentRoll;
	public:
		virtual void rollDice() = 0;
		int getRoll() const;
		Dice & getSource() override;
		Subject::NotifierType getType() override;
};

class LoadedDice : public Dice {

public:
	void rollDice() override;
};

class FairDice : public Dice {

public:
	void rollDice() override;
};

#endif		
