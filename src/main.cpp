#include <iostream>
#include <vector>

using std::cout;
using std::string;
using std::vector;

class Card {
private:

public:

};

class CharacterCard : public Card {
private:
    string _name;
    int _health;
    int _attack;
    string _startingItemName;
public:


}

class Player {
private:
    string _name;
public:
    Player(string name) : _name(name) {

    }
};

class Board {
    
};

class Game {
private:
    int currentPlayer;
    vector<Player*> _players;
public:
    Game() {

    }

    ~Game() {
        for (const auto& p : players) delete p;
    }

    int playerCount() { return _players.size(); }
};

int main() {

}