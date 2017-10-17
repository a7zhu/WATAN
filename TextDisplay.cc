#include "TextDisplay.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

struct posn {
    int y;
    int x;
    posn(int a, int b):y(a),x(b){}
};

TextDisplay::TextDisplay() { // read from file "watanBoard.txt"
	ifstream file("watanBoard.txt"); 
	string line;
	// Convert the contents into a vector of strings
	while(getline(file,line)) {
		theDisplay.push_back(line);
	}
	// Set posn's for Tile, Criterion, and Goals
	setPositions();
}

void TextDisplay::setPositions() {
        int numLines;
        int currLine = 0;
        int y;
        int x;
        int i;
        string line;
        // Note: Each file contains lines of TWO integers, representing y and x values (in that order)
        // ADD TILE POSITIONS
        ifstream tilePositions("tilePositions.txt"); // open tilePositions
        numLines = 19; // number of lines in file
        while(currLine < numLines) {
                getline(tilePositions, line);
                istringstream t {line};
                t >> i;
                y = i; // set y-value to first number
                t >> i;
                x = i; // set x-value to next number
                tilePos.push_back(posn(y,x)); // push a posn structure constructed using those y,x values
                ++currLine;
        }
        tilePositions.close();
        currLine = 0;
        // ADD CRITERION POSITIONS
        ifstream critPositions("criterionPositions.txt"); // open critPositions
        numLines = 54; // change number of lines
        while(currLine < numLines) {
                getline(critPositions, line);
                istringstream c {line};
                c >> i;
                y = i;
                c >> i;
                x = i;
                critPos.push_back(posn(y,x));
                ++currLine;
        }
        critPositions.close();
        currLine = 0;

        // ADD GOAL POSITIONS
        ifstream goalPositions("goalPositions.txt"); // open goalPositions
        numLines = 72; // change number of lines
        while(currLine < numLines) {
                getline(goalPositions, line);
                istringstream g {line};
                g >> i;
                y = i;
                g >> i;
                x = i;
                goalPos.push_back(posn(y,x));
                ++currLine;
        }
        goalPositions.close();
}


// IMPLEMENT UPDATING THE VALUE
void TextDisplay::updateTile(int pos, int val, Resources r, bool printGeese) {
	int y_resource = tilePos[pos].y;
	int x_resource = tilePos[pos].x - 1;
	int x_value = x_resource + 2;
	int y_index = y_resource - 1;
	int geese_index = y_index + 2;
	// Add the Resource text
	if (printGeese) {
		theDisplay[geese_index].replace(x_resource, 5, "GEESE");
	}
	else {
		theDisplay[geese_index].replace(x_resource, 5, "     ");
	}
	if(r == Resources::CAFFEINE) {
		theDisplay[y_index].replace(x_resource, 8, "CAFFEINE");
	}
	else if(r == Resources::LAB) {
		theDisplay[y_index].replace(x_resource, 3, "LAB");
	}
	else if(r == Resources::LECTURE) {
		theDisplay[y_index].replace(x_resource, 7, "LECTURE");
	}
	else if(r == Resources::STUDY) {
		theDisplay[y_index].replace(x_resource, 5, "STUDY");
	}
	else if(r == Resources::TUTORIAL) {
		theDisplay[y_index].replace(x_resource, 8, "TUTORIAL");
	}
	else {
		theDisplay[y_index].replace(x_resource, 7, "NETFLIX");
	}
	// Add the Value number
	if(val < 10) {
		if(val == 7) {
		}
		else {
			theDisplay[y_index+1].replace(x_value+1, 1, to_string(val));
		}
	}
	else {
		theDisplay[y_index+1].replace(x_value, 2, to_string(val));
	}
}

void TextDisplay::updateCriterion(int pos, Criterion::Improvement i, string color) {
	int y = critPos[pos].y;
	int x_color = critPos[pos].x - 1;
	int x_rank = x_color + 1;
	int y_index = y - 1;
	// First add letter representing Color
	if(color == "BLUE") {
		theDisplay[y_index].replace(x_color, 1, "B");
	}
	else if(color == "RED") {
		theDisplay[y_index].replace(x_color, 1, "R");
	}
	else if(color == "ORANGE") {
		theDisplay[y_index].replace(x_color, 1, "O");
	}
	else {
		theDisplay[y_index].replace(x_color, 1, "Y");
	}
	// Add letter representing rank of the Criterion
	if(i == Criterion::Improvement::Assignment) {
		theDisplay[y_index].replace(x_rank, 1, "A");
	}
	else if(i == Criterion::Improvement::Midterm) {
		theDisplay[y_index].replace(x_rank, 1, "M");
	}
	else {
		theDisplay[y_index].replace(x_rank, 1, "E");
	}
}

void TextDisplay::updateGoal(int pos, string color) {
	int y = goalPos[pos].y;
	int x_color = goalPos[pos].x - 1;
	int x_achievement = x_color + 1;
	int y_index = y - 1;
	// First add letter representing Color
	if(color == "BLUE") {
		theDisplay[y_index].replace(x_color, 1, "B");
	}
	else if(color == "RED") {
		theDisplay[y_index].replace(x_color, 1, "R");
	}
	else if(color == "ORANGE") {
		theDisplay[y_index].replace(x_color, 1, "O");
	}
	else {
		theDisplay[y_index].replace(x_color, 1, "Y");
	}
	// Add 'A' for "Achievement"
	theDisplay[y_index].replace(x_achievement, 1, "A");
}

void TextDisplay::notify(Subject &whoNotified) {
	Player *p;
	int pos;
	string color;
	Subject::NotifierType type = whoNotified.getType();
	if(type == Subject::NotifierType::CRITERION) {
		Criterion &crit = dynamic_cast<Criterion&>(whoNotified.getSource());
		p = crit.getOwner();
		pos = crit.getPosition();
		Criterion::Improvement i = crit.getImprovement();
		color = p->getName();
		updateCriterion(pos, i, color);
	}
	else if(type == Subject::NotifierType::TILE) {
		Tile &t = dynamic_cast<Tile&>(whoNotified.getSource());
		int val = t.getValue();
		bool printGeese = false;
		if (t.isOverrun()) printGeese = true;
		pos = t.getPosition();
		Resources r = t.getResource();
		updateTile(pos, val, r, printGeese);
	}
	else if(type == Subject::NotifierType::GOAL) {
		Goal &g = dynamic_cast<Goal&>(whoNotified.getSource());
		pos = g.getPosition();
		p = g.getOwner();
		color = p->getName();
		updateGoal(pos, color);
	}
}

SubscriptionType TextDisplay::subType() const {
	return SubscriptionType::DISPLAY;
}

ostream &operator<<(ostream &out, const TextDisplay &td) {
	int numLines = td.theDisplay.size();
	for(int i = 0; i < numLines; ++i) {
		out << td.theDisplay[i] << endl;
	}
	return out;
}

TextDisplay::~TextDisplay() {}
