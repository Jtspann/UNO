#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

enum actions { SKIP = 10, REVERSE, PLUS2, WILD, PLUS4, DRAW, OUT_OF_BOUNDS, CW, CCW };

struct card {
	string color{};
	int value{};
	string displayValue{};
	card(string c, int v) { // Default constructor; It gets called every time a new card is made.
		color = c;
		value = v;
		switch (v) {
		case SKIP:
			displayValue = "Skip";
			break;
		case REVERSE:
			displayValue = "Reverse";
			break;
		case PLUS2:
			displayValue = "+2";
			break;
		case WILD:
			displayValue = "Wild";
			break;
		case PLUS4:
			displayValue = "+4";
			break;
		default:
			displayValue = to_string(value);
			break;
		}
	}
};

struct player {
	string name{};
	vector<card> hand{};
	bool missedCall{ false };
	bool dealCalled{ false };
	int prvSize{};
};

void PressEnter();
void populate(std::vector<card>& cards);
void dealCards(int num, vector<card>& deck, vector<card>& hand);
int nextPlayer(int amount, int dir, int cur);
int skip(int amount, int dir, int cur);
void wild(card& wildCard);
bool canPlay(card hand, card top);
bool playing(vector<player>& players);



int main() {

	srand(time(NULL));

reName: // Input names for the players.
	cout << "Input names, pressing 'Enter' between each each name. When finished press '-' on a new line.\n";
	vector<string> names(1);
	while (names.back() != "-") {
		if (names.at(0) == "") {
			names.pop_back();
		}
		string temp{};
		cin >> temp;
		names.push_back(temp);
	}
	names.pop_back();
	if ((names.size() < 2) || (names.size() > 10)) { // Makes sure there are the proper amount of players.
		cout << "\nThat is an invalid number of players. There can be between 2 and 10 players. Please try again.";
		Sleep(10000);
		system("CLS");
		goto reName; 
	} 
	system("CLS");

	vector<card> deck{};
	vector<card> stack{};
	populate(deck);
	vector<player> p;
	for (auto i : names) { // Makes players.
		p.push_back({ i,vector<card>{} }); 
	}
	for (auto& i : p) { // Gives all players cards.
		dealCards(7, deck, i.hand);
		i.prvSize = i.hand.size();
	}
	dealCards(1, deck, stack);

	int dir = CW;
	int pNum{};
	while (playing(p)) {

		system("CLS");

		cout << "It's your turn, " << p[pNum].name << ". Press 'Enter' to continue.";
		PressEnter();

		if (p[pNum].dealCalled) { // Check for pending cards.
			dealCards(2, deck, p[pNum].hand); 
			p[pNum].dealCalled = false;
		}

	retry: //If a player trys to play a card within range and fails, it still marks them as not calling uno.
		system("CLS");
		cout << "Current Player: " << p[pNum].name << "\n";
		cout << "Top Card: " << stack.back().color << " " << stack.back().displayValue;
		cout << "\nColor:\t";
		for (auto i : p[pNum].hand) {
			cout << i.color << "\t";
		}
		cout << "\nNumber:\t";
		for (auto i : p[pNum].hand) {
			cout << i.displayValue << "\t";
		}
		cout << "\nIndex:\t";
		for (size_t i = 0; i < p[pNum].hand.size(); i++) {
			cout << i + 1 << "\t";
		}
		if (p[pNum].hand.size() == 2u) {
			cout << "\nType 'u' after your selection to call UNO! Example: 1u";
		}
		for (auto i : p) {
			if (i.missedCall) {
				cout << "\nA player failed to call UNO! Type 'c' to call them out! Example: 4c";
				break;
			}
		}
		cout << "\n\nPlaying Card# ";
		string sIndex;

		cin >> sIndex;		
		size_t unoCall = sIndex.find("u");
		if ((p[pNum].hand.size() == 2u) && (p[pNum].hand.size() < static_cast<size_t>(p[pNum].prvSize))) {
			if (unoCall == sIndex.npos) { p[pNum].missedCall = true; } // If they have 2 cards and there is no 'u', they get marked as having missed it.
			else { sIndex.erase(unoCall); }
		}
		if (unoCall != sIndex.npos) {
			sIndex.erase(unoCall);
		}
		size_t playerCall = sIndex.find("c");
		if (playerCall != sIndex.npos) { // If it finds a c...
			for (auto &i : p) {
				if (i.missedCall == true) { // ...and a player has a missed call... 
					i.dealCalled = true; // ...they get marked as needing cards.
					i.missedCall = false;
				}
			}
			sIndex.erase(playerCall); // Remove 'c' from string for index conversion.
		}
		if (sIndex == "-") { 
			if (deck.empty()) { // Make sure there's something to pull before trying.
				while (stack.size() > 1) { // Loop will put used card back in the main deck.
					deck.push_back({ stack.front().color, stack.front().value });
					stack.erase(stack.begin());
				}
			}
			dealCards(1, deck, p[pNum].hand);
			goto retry;
		}
		else {
			try { // If it's an invalid index or cannot convert to an integer, an error will be thrown. 
				int index = stoi(sIndex) - 1;
				if (canPlay(p[pNum].hand.at(index), stack.back())) {
					bool doSkip{ false };
					switch (p[pNum].hand.at(index).value) {
					case SKIP:
						doSkip = true;
						break;
					case REVERSE:
						if (dir == CW) {
							dir = CCW;
						}
						else if (dir == CCW) {
							dir = CW;
						}
						break;
					case PLUS2:
						dealCards(2, deck, p[nextPlayer(p.size(), dir, pNum)].hand);
						doSkip = true;
						break;
					case WILD:
						wild(p[pNum].hand.at(index));
						break;
					case PLUS4:
						wild(p[pNum].hand.at(index));
						dealCards(4, deck, p[nextPlayer(p.size(), dir, pNum)].hand);
						doSkip = true;
						break;
					case DRAW:
						dealCards(1, deck, p[pNum].hand);
						break;
					}
					p[pNum].prvSize = p[pNum].hand.size();
					stack.push_back(p[pNum].hand.at(index)); // Moves used card back into stack.
					p[pNum].hand.erase(p[pNum].hand.begin() + index);
					if (doSkip) { pNum = skip(p.size(), dir, pNum); }
					else { pNum = nextPlayer(p.size(), dir, pNum); }
					system("CLS");
				}
				else {
					cout << "\nThat card cannot be used. Please, try again.";
					Sleep(3000);
					system("CLS");
					goto retry;
				}
			}
			catch (...) { // OOB or invalid conversion error gets caught, player is directed to try again.
				cout << "Invalid input. Please, try again.";
				Sleep(3000);
				system("CLS");
				goto retry;
			}
		}

		for (auto i : p) { // Since the loop only breaks once somebody has no cards, a quick scan can figure out who that is.
			if (i.hand.size() == 0) {
				cout << i.name << " wins!";
				Sleep(5000);
				break;
			}
		}
	}
}

void PressEnter() { // Waits until a player presses enter to continue.
	string temp{ "~" }, temp1{};
	getline(cin, temp1);
	while (temp != "") {
		getline(cin, temp, '\n');
	};
	system("CLS");
}

void populate(std::vector<card>& cards) { 
	for (int i = 0; i < 8; i++) { // Creates 2 sets of each color from 1 to +2 (12)
		for (int j = 1; j < 13; j++) {
			switch (i % 4) {
			case 0:
				cards.push_back({ "Red",j});
				break;
			case 1:
				cards.push_back({ "Green",j });
				break;
			case 2:
				cards.push_back({ "Blue",j });
				break;
			case 3:
				cards.push_back({ "Yellow",j });
				break;
			}
		}
	}
	cards.push_back({ "Red",0 }); // A 0 for every color.
	cards.push_back({ "Green",0 });
	cards.push_back({ "Blue",0 });
	cards.push_back({ "Yellow",0});
	for (int i = 0; i < 4; i++) { // Adds 4 wild cards and 4 +4 cards.
		cards.push_back({ "Wild", WILD });
		cards.push_back({ "Wild", PLUS4 });
	}
}

void dealCards(int num, vector<card>& deck, vector<card>& hand) { // Loop will push a random card from the deck to the hand, then delete it from the deck.
	for (int i = 0; i < num; i++) {
		int index = rand() % deck.size();
		hand.push_back({ deck[index].color,deck[index].value });
		deck.erase(deck.begin() + index);
	}
}

int nextPlayer(int amount, int dir, int cur) {
	amount -= 1;
	if (dir == CW){ //The order goes clockwise and restarts at the first person.
		if (cur < amount) {
			cur += 1;
		}
		else {
			cur = 0;
		}
	}
	if (dir == CCW) { //The order goes counter clockwise and restarts at the last person.
		if (cur > 0){
			cur -= 1;
		}
		else {
			cur = amount;
		}
	}
	return cur;
}

int skip(int amount, int dir, int cur) {
	int next{};
	next = nextPlayer(amount, dir, cur); 
	next = nextPlayer(amount, dir, next); // Running the next player two times.
	return next; // Returns index of next player back into the main funtion.
}

void wild(card& wildCard){
	system("CLS");
	int inputWild{}; // Prompts player to enter a number assigned to a color.
	cout << "1) Red\n2) Blue\n3) Green\n4) Yellow\n";
	cin >> inputWild;
	if(inputWild == 1){
	wildCard.color = "Red";
	}
	if(inputWild == 2){
	wildCard.color = "Blue";
	}
	if(inputWild == 3){
	wildCard.color = "Green";
	}
	if(inputWild == 4){
	wildCard.color = "Yellow";
	}
	system("CLS");
}

bool canPlay(card hand, card top) {  // Compare the color and number of hand card to top card.
	if (hand.color == top.color) {
		return true;
	}
	if (hand.value == top.value) {
		return true;
	}
	if (hand.color == "Wild") {
		return true;
	}
	else {
		return false;
	}
}

bool playing(vector<player>& players) { // Tests to see if all players have more than 0 cards in their hands.
	for (auto i : players) {
		if (i.hand.size() == 0) {
			return false;
		}
	}
	return true;
}