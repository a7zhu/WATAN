#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include <iostream>
#include <vector>
#include <string>
#include "Observer.h"
#include "Goal.h"
#include "Criterion.h"
#include "Tile.h"
#include "Resources.h"
struct posn;
class Tile;

class TextDisplay: public Observer {
  std::vector<std::string> theDisplay;
  std::vector<posn> tilePos;
  std::vector<posn> critPos;
  std::vector<posn> goalPos;
 public:
  TextDisplay();
  void setPositions();
  void updateTile(int n, int val, Resources r, bool printGeese = false); // Update Tile w/ val and resource
  void updateCriterion(int n, Criterion::Improvement, string color); // Update Criterion at position n
  void updateGoal(int n, string color); // Update Goal at position n
  void notify(Subject &whoNotified) override;
  SubscriptionType subType() const override;

  ~TextDisplay();

  friend std::ostream &operator<<(std::ostream &out, const TextDisplay &td);
};

#endif
