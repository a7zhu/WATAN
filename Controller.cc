#include "Controller.h"
#include "Board.h"

using namespace std;

Controller::Controller(){
	players.emplace_back(Player("BLUE"));
	players.emplace_back(Player("RED"));
	players.emplace_back(Player("ORANGE"));
	players.emplace_back(Player("YELLOW"));
}
	
void Controller::saveGame(){
	ofstream file {"backup.sv"};
	ostringstream oss;
	oss << turnNum <<endl;
	for(int i=0; i< 4; ++i) oss << players[i].exportInfo() << endl;
	oss << board->exportInfo() << endl;
	file << oss.str();
	throw "Game over!";
}
	
int Controller::readPosition(istream & in, ostream & out, int lo, int hi, bool arrow){
	if(arrow) out << ">";
	string input;
	int pos;
	while(in >> input){
		istringstream ss {input};
		if(ss >> pos && pos >=lo && pos <= hi){
			return pos;
		}else{
			out << "Invalid command." << endl;
			if (!arrow) return -1;
			out << ">";
		}			
	}
	if (in.eof()) {
		in.clear();
		saveGame();
	}	
	return 0;
}

void Controller::startGame(){
	board = make_shared<randomBoard>();
	int size = players.size();
	for (int i = 0; i < size; ++i) {
		board->attach(&players[i]);
	}
}


void Controller::loadBoardFromFile(string filename){
	ifstream file {filename};
	string info;
	getline(file, info);
	
	board = make_shared<savedBoard>();
	int size = players.size();
	for (int i = 0; i < size; ++i) {
		board->attach(&players[i]);
	}
	board->loadBoard(info);
}


static string parsePlayerInfo(string info){
	istringstream iss {info};
	ostringstream useful;
	string catcher;
	for(int i=0;i<5;++i)iss >> catcher;
	while(iss >> catcher) useful << catcher << " ";
	return useful.str();
}

void Controller::loadGameFromFile(string filename){
	
	board = make_shared<savedBoard>();
	
	int size = players.size();
	for (int i = 0; i < size; ++i) {
		board->attach(&players[i]);
	}
	
	ifstream file{filename};
	string line;
	
	//Get turnNum info from file
	getline(file, line);
	istringstream iss{line};
	iss >> turnNum;	
	
	//Get 4 Player info from file
	vector<string> playersInfo;
	for(int i=0; i<4; ++i){
		getline(file, line);
		playersInfo.emplace_back(line);
	}
	//Get Board info from tile
	string boardInfo; 
	getline(file, boardInfo);
	
	//Pass Player info to Players
	for(int i=0; i<4; ++i) players[i].loadStats(playersInfo[i]);
	//Pass Board info to Board
	board->loadBoard(boardInfo);
	//Pass Player info to Board
	for(int i=0; i<4; ++i) board->loadOwner(&players[i], parsePlayerInfo(playersInfo[i]));
}



	
void Controller::getSetupMove(istream & in, ostream & out ){
	vector<int> order {0,1,2,3,3,2,1,0};
	int pos;
	for(int i=0; i<8;++i){
		do{
			out << "Student " << players[order[i]].getName() << ", where do you want to complete an Assignment?" << endl;
			pos = readPosition(in,out,0,53);
		}while(!board->placeCriterion(&players[order[i]], pos));
	}	
	out << *board;
}
	
// beginning of the turn command
void Controller::getBOTmove(istream & in, ostream & out){
		out << "Student " << players[turnNum].getName() <<  "'s turn." << endl;
		out << players[turnNum].printStatus() << endl;
		string cmd;
		out << ">";
		while(in >> cmd){
			if (cmd == "load") {
				players[turnNum].setDiceToLoad();
			} else if (cmd == "board") {
			out << *board;
			} else if (cmd == "fair") {
				players[turnNum].setDiceToFair();
			} else if (cmd == "roll") {
				try 
				{
					board->roll(&players[turnNum]);
					return;
				}
				catch (char const* error)
				{
					saveGame();
				}
			} else {
				out << "Invalid command." << endl;
			}
			out << ">";
		}
		if (in.eof()) {
		in.clear();
		saveGame();
	}
}

static Resources resourceLookup(string s){
	if(s=="CAFFEINE") return Resources::CAFFEINE;
	else if(s=="LAB") return Resources::LAB;
	else if(s=="LECTURE") return Resources::LECTURE;
	else if(s=="STUDY") return Resources::STUDY;
	else return Resources::TUTORIAL;
}
	
// End of the turn command 
void Controller::getEOTmove(istream & in, ostream & out){
	string cmd;
	out << ">";
	while(in >> cmd){
		if (cmd == "board") {
			out << *board;
		} else if (cmd == "status") {
			for(int i=0;i<4;++i){
				out << players[i].printStatus()<<endl;
			}
		} else if (cmd == "criteria") {
			out << players[turnNum].printCriteria()<<endl;
		} else if (cmd == "achieve") {
			int pos = readPosition(in, out, 0, 71, false);
			if (pos != -1) {
				board->achieveGoal(&players[turnNum], pos);
			}
		} else if (cmd == "complete") {
			int pos = readPosition(in, out, 0, 53, false);
			if (pos != -1) {
				try {
					board->completeCriterion(&players[turnNum], pos);
				}
				catch (int won) {
					out << "Student " << players[turnNum].getName() << " has won!" << endl;
					throw;
				}
			}
		} else if (cmd == "improve") {
			int pos = readPosition(in, out, 0, 53, false);
			if (pos != -1) {
				try {
				board->improveCriterion(&players[turnNum], pos);
				}
				catch (int won) {
					out << "Student " << players[turnNum].getName() << " has won!" << endl;
					throw;
				}
			}
		} else if (cmd == "trade") {
			string colour, give, take;
			while(in >> colour){
				if (in.eof()) {
					in.clear();
					saveGame();
				}
				in >> give;
				if (in.eof()) {
					in.clear();
					saveGame();
				}
				in >> take;
				if (in.eof()) {
					in.clear();
					saveGame();
				}
				if((colour==players[turnNum].getName()) || 
					!(colour=="BLUE" || colour=="RED" || colour=="YELLOW" || colour == "ORANGE")){
					out << "Invalid command." << endl;
					break;
				}
				if(!(give=="CAFFEINE" || give=="LAB" || give=="LECTURE" || give=="STUDY" || give == "TUTORIAL")){
					out << "Invalid command." << endl;
					break;
				}
				if(!(take=="CAFFEINE" || take=="LAB" || take=="LECTURE" || take=="STUDY" || take=="TUTORIAL")){
					out << "Invalid command." << endl;
					break;
				}	
				Player * p;
				Resources r1, r2;
				if(colour=="BLUE") p=&players[BLUE];
				else if(colour == "RED") p=&players[RED];
				else if(colour == "YELLOW") p =&players[YELLOW];
				else p = &players[ORANGE];
						
				r1 = resourceLookup(give);
				r2 = resourceLookup(take);

				try 
				{
					players[turnNum].trade(p, r1, r2);
				}
				catch (char const* error) 
				{
					saveGame();
				}
				break;
			}
			if (in.eof()) {
				in.clear();
				saveGame();
			}
		} else if (cmd == "next") {
			if(turnNum==3) turnNum =0;
			else ++turnNum;
			return;
		} else if (cmd == "save") {
			string filename;
			while(in >> filename){
				ofstream file {filename};
				if (file){
					ostringstream oss;
					oss << turnNum << endl;
					for(int i=0; i< 4; ++i) oss << players[i].exportInfo() << endl;
					oss << board->exportInfo() << endl;
					file << oss.str();
					break;
				}else{
					out << "Invalid command."<<endl;
					out << ">";
				}
			}
			if(in.eof()) {
				in.clear();
				saveGame();
			}
		} else if (cmd == "help") {
			out<<"Valid commands:"<<endl;
			out<<"board" <<endl;
			out<<"status"<<endl;
			out<<"criteria"<<endl;
			out<<"achieve <goal>"<<endl;
			out<<"complete <criterion>"<<endl;
			out<<"improve <criterion>"<<endl;
			out<<"trade <colour> <give> <take>"<<endl;
			out<<"next"<<endl;
			out<<"save <file>"<<endl;
			out<<"help"<<endl;
			out << "Costs:" << endl;
			out << "Criterion Assignment: 1 CAFFEINE, 1 LAB, 1 LECTURE, 1 TUTORIAL" << endl;
			out << "Criterion Midterm: 2 LECTURE, 3 STUDY" << endl;
			out << "Criterion Exam: 3 CAFFEINE, 2 LAB, 2 LECTURE, 1 TUTORIAL, 2 STUDY" << endl;
			out << "Goal Achievement: 1 TUTORIAL, 1 STUDY" << endl;
		} else {
			out << "Invalid command." << endl;
		}
		out << ">";
	}
	if (in.eof()) {
		in.clear();
		saveGame();
	}
}

void Controller::restartGame(){
	startGame();
	turnNum = 0;
	players.clear();
	players.emplace_back(Player("BLUE"));
	players.emplace_back(Player("RED"));
	players.emplace_back(Player("ORANGE"));
	players.emplace_back(Player("YELLOW"));
}
