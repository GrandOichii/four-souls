#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <deque>
#include <map>
#include <stack>
#include <random>
#include <stdlib.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "cards.hpp"
#include "types.hpp"
#include "util.hpp"

using std::string;

const int MIN_PLAYER_COUNT = 2;
const int MAX_PLAYER_COUNT = 4;
const int SOULS_TO_WIN = 4;
const int STARTING_COIN_AMOUNT = 3;
const int STARTING_LOOT_AMOUNT = 2;

struct PlayerBoardState {
    bool characterActive;
    std::pair<string, bool> playerCard;
    std::vector<std::pair<string, bool>> board;
    std::vector<string> hand;
};

struct StackMememberState {
    string message;
    string cardName;
    bool isCard;
};

struct MatchState {
    vector<PlayerBoardState> boards;
    vector<StackMememberState> stack;
    int currentI;
    int priorityI;

    int lootDeckCount;
    int treasureDeckCount;
    int monsterDeckCount;

    vector<string> lootDiscard;
    vector<string> treasureDiscard;
    vector<string> monsterDiscard;
};

class Player {
private:
    CharacterCard* _characterCard;
    std::string _name;

    int _baseMHealth;
    int _health;

    int _baseAttack;
    int _id;

    int _soulCount;

    int _coinCount;
    int _additionalCoins;

    bool _characterActive;
    int _startTurnLootAmount;

    std::vector<CardWrapper*> _board; // dont forget to delete these!
    std::vector<LootCard*> _hand;
public:
    Player(std::string name, CharacterCard* card, int id) :
        _name(name),
        _id(id),
        _characterCard(card) 
    {
        this->_baseMHealth = card->health();
        this->_health = this->_baseMHealth;

        this->_baseAttack = card->attack();
        auto w = new CardWrapper(card->startingItem(), _id);
        w->tap();
        this->_board.push_back(w);
        this->_characterActive = false;
        this->_startTurnLootAmount = 1;

        this->_soulCount = 0;

        this->_coinCount = 0;
        this->_additionalCoins = 0;
    }

    virtual ~Player() {
        for (const auto& w : _board) delete w;
    }


    void print() {
        std::cout << _name << " (" << _id << ")" << std::endl;
        std::cout << "Board: " << std::endl;
        for (const auto& w : _board)
            std::cout << w->card()->name() << " (" << w->id() << ")" << std::endl;
    }

    LootCard* takeCard(int cardI) {
        return *_hand.erase(_hand.begin()+cardI-1);
    }

    std::vector<CardWrapper*> board() { return _board; }
    std::vector<LootCard*> hand() { return _hand; }

    bool characterActive() { return _characterActive; }

    void rechargeCharacter() {
        this->_characterActive = true;
    }

    void rechargeCards() {
        for (auto& w : _board)
            w->recharge();
    }
    
    void recharge() {
        this->rechargeCharacter();
        this->rechargeCards();
    }

    void pushTable(lua_State* L) {
        lua_newtable(L);
        l_pushtablestring(L, "name", this->_name);
        l_pushtablenumber(L, "id", (float)this->_id);
        l_pushtablenumber(L, "startTurnLootAmount", (float)this->_startTurnLootAmount);
    }

    virtual string promptAction() = 0;

    string name() { return _name; }
    int id() { return _id; }
    int soulCount() { return _soulCount; }
    CharacterCard* characterCard() { return _characterCard; }

    int coinCount() { return _coinCount; }
    void addCoins(int amount) { this->_coinCount += amount + _additionalCoins; }
    void removeCoins(int amount) { this->_coinCount -= amount; }

    void incAdditionalCoins() { _additionalCoins++; }
    void decAdditionalCoins() { _additionalCoins--; }

    void addLootCards(vector<LootCard*> cards) {
        for (const auto& c : cards)
            this->_hand.push_back(c);
    }

    PlayerBoardState getState() {
        PlayerBoardState result;
        result.playerCard.first = _characterCard->name();
        result.playerCard.second = _characterActive;
        for (const auto& w : _board)
            result.board.push_back(std::make_pair(w->card()->name(), w->isActive()));
        for (const auto& c : _hand)
            result.hand.push_back(c->name());
        return result;
    }    
};

class ScriptedPlayer : public Player {
private:
    std::stack<string> _actions;
public:
    ScriptedPlayer(std::string name, CharacterCard* card, int id, string actions) :
        Player(name, card, id)
    {
        auto split = str::split(actions, "\n");
        for (int i = split.size()-1; i >= 0; i--)
            _actions.push(split[i]);
    }

    string promptAction() {
        if (_actions.empty()) return ACTION_PASS;
        auto result = _actions.top();
        _actions.pop();
        return result;
    }
};

struct StackEffect {
    string funcName;
    Player* player;
    CardWrapper* card;

    StackEffect(string funcName, Player* player, CardWrapper* card) :
        funcName(funcName),
        player(player),
        card(card) {}

    StackEffect() {}

    StackMememberState getState() {
        StackMememberState result;
        result.message = funcName;
        result.isCard = card;
        if (card) result.cardName = card->card()->name();
        return result;
    }
};

class Match {
private:
    int _lastID = 0;
    bool _running = false;
    lua_State *L;
    int _turnCounter = 0;

    std::default_random_engine rng = std::default_random_engine {};
    std::vector<std::pair<CharacterCard*, bool>> _characterPool;

    std::vector<Player*> _players;
    int _currentI;
    int _nextI;
    int _priorityI;
    Player* _activePlayer;

    std::deque<LootCard*> _lootDeck;
    std::deque<LootCard*> _lootDiscard;

    std::deque<TrinketCard*> _treasureDeck;
    std::deque<TrinketCard*> _treasureDiscard;

    std::vector<StackEffect> _stack;
    std::stack<string> _eotDefers;
    std::stack<StackEffect> _eotDeferredTriggers;

    std::map<string, std::function<void(Player*, std::vector<string>)>> _actionMap = {
        {ACTION_PLAY_LOOT, [this](Player* player, std::vector<string> args){
            auto cardI = atoi(args[1].c_str());
            auto card = player->takeCard(cardI);
            if (!card->isTrinket()) this->_lootDiscard.push_back(card);
            auto wrapper = new CardWrapper(card, this->newCardID());
            this->pushToStack(StackEffect(
                "_popLootStack",
                player,
                wrapper
            ));
            this->_lootStack.push(std::make_pair(card, player));
        }}
    };

    std::stack<std::pair<LootCard*, Player*>> _lootStack;
public:
    Match() {
        this->rng.seed(rand());
    }

    ~Match() {
        for (const auto& p : _players) delete p;

        lua_close(this->L);
    }

    CardWrapper* cardWithID(int id) {
        for (const auto& p : _players)
            for (const auto& w : p->board())
                if (w->id() == id)
                    return w;
        throw std::runtime_error("no card with id " + std::to_string(id));
    }

    Player* findOwner(CardWrapper* card) {
        for (const auto& p : _players)
            for (const auto& w : p->board())
                if (w == card)
                    return p;
        throw std::runtime_error("can't find owner of card " + card->card()->name() + " [" + std::to_string(card->id()) + "]");
    } 

    void shuffleLootDiscardIntoMain() {
        while (!_lootDiscard.empty()) {
            _lootDeck.push_back(_lootDiscard.back());
            _lootDiscard.pop_back();
        }
        this->shuffleLootDeck();
    }

    LootCard* getTopLootCard() {
        if (!_lootDeck.size()) this->shuffleLootDiscardIntoMain();
        if (_lootDeck.empty()) return nullptr;
        return _lootDeck.back();
    }

    vector<LootCard*> getTopLootCards(int amount) {
        vector<LootCard*> cards;
        while (amount) {
            auto card = getTopLootCard();
            if (!card) {
                this->log("The loot deck is empty");
                break;
            }
            cards.push_back(card);
            // cards.push_back(_lootDeck.back());
            _lootDeck.pop_back();
            amount--;
        }
        return cards;
    }

    static int wrap_getOwner(lua_State *L) {
        if (lua_gettop(L) != 1) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        match->_stack.back().player->pushTable(L);
        return 1;
    }

    static int wrap_lootCards(lua_State *L) {
        if (lua_gettop(L) != 3) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        if (!lua_isnumber(L, 2)) {
            lua_err(L);
            exit(1);
        }
        auto pid = (int)lua_tonumber(L, 2);
        
        if (!lua_isnumber(L, 3)) {
            lua_err(L);
            exit(1);
        }
        auto amount = (int)lua_tonumber(L, 3);
        Player* player = nullptr;
        for (const auto& p : match->_players) {
            if (p->id() == pid) {
                player = p;
                break;
            }
        }
        if (!player) throw std::runtime_error("no player with id " + std::to_string(pid));
        match->log(player->name() + " loots " + std::to_string(amount) + " cards");
        auto cards = match->getTopLootCards(amount);
        player->addLootCards(cards);
        return 0;
    }

    static int wrap_popLootStack(lua_State* L) {
        if (lua_gettop(L) != 1) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        // std::cout << "Popping loot stack" << std::endl;
        auto last = match->_lootStack.top();
        match->_lootStack.pop();
        auto card = last.first;
        auto player = last.second;
        //  TODO execute the loot card
        return 0;
    }

    static int wrap_deferEOT(lua_State *L) {
        if (lua_gettop(L) != 4) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        if (!lua_isnumber(L, 2)) {
            lua_err(L);
            exit(1);
        }
        auto cardID = (int)lua_tonumber(L, 2);
        auto card = match->cardWithID(cardID);
        auto owner = match->findOwner(card);
        if (!lua_isstring(L, 3)) {
            lua_err(L);
            exit(1);
        }
        auto funcName = (string)lua_tostring(L, 3);
        if (!lua_isboolean(L, 4)) {
            lua_err(L);
            exit(1);
        }
        auto isTrigger = (bool)lua_toboolean(L, 4);
        if (isTrigger) {
            StackEffect effect;
            effect.funcName = funcName;
            effect.card = card;
            effect.player = owner;
            match->_eotDeferredTriggers.push(effect);
        } else match->_eotDefers.push(funcName);
        return 0;
    }

    static int wrap_this(lua_State *L) {
        if (lua_gettop(L) != 1) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        match->_stack.back().card->pushTable(match->L);
        return 1;
    }

    void pushEOTDeferredTriggers() {
        while (!_eotDeferredTriggers.empty()) {
            this->pushToStack(_eotDeferredTriggers.top());
            _eotDeferredTriggers.pop();
        }
    }

    void execEOTDefers() {
        while (!_eotDefers.empty()) {
            this->execFunc(_eotDefers.top());
            _eotDefers.pop();
        }
    }

    void setupLua() {
        this->L = luaL_newstate();
        // connect common libs
        luaL_openlibs(L);
        // connect functions
        // lua_register(L, "foo", wrap_foo);
        lua_register(L, "getOwner", wrap_getOwner);
        lua_register(L, "lootCards", wrap_lootCards);
        lua_register(L, "deferEOT", wrap_deferEOT);
        lua_register(L, "_popLootStack", wrap_popLootStack);
        lua_register(L, "this", wrap_this);

        // load card scripts
        for (const auto& card : _lootDeck) {
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        for (const auto& card : _treasureDeck) {
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        for (const auto& pair : _characterPool) {
            auto character = pair.first;
            auto card = character->startingItem();
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        std::cout << "All scripts loaded!" << std::endl;
    }

    static void lua_err(lua_State *L) {
        string errormsg = lua_tostring(L, -1);
        std::cout << errormsg << std::endl;
    }

    void execScript(string script) {
        int r = luaL_dostring(L, script.c_str());
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
    }

    void execFunc(string funcName) {
        lua_getglobal(L, funcName.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_err(L);
            exit(1);
        }
        lua_pushlightuserdata(L, this);
        int r = lua_pcall(L, 1, 0, 0);
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
    }

    bool execCheck(string funcName) {
        lua_getglobal(L, funcName.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_err(L);
            exit(1);
        }
        lua_pushlightuserdata(L, this);
        int r = lua_pcall(L, 1, 1, 0);
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
        if (!lua_isboolean(L, -1)) {
            lua_err(this->L);
            exit(1);
        }
        return (bool)lua_toboolean(L, -1);
    }

    void addToCharacterPool(CharacterCard* card) {
        _characterPool.push_back(std::make_pair(card, true));
    }

    //  TODO remove the actions part
    Player* addPlayer(std::string name, CharacterCard* character, string actions) {
        for (const auto& p : _players)
            if (p->name() == name)
                return nullptr;
        //  TODO change this to a player with a port
        auto result = new ScriptedPlayer(name, character, newCardID(), actions);
        result->addCoins(STARTING_COIN_AMOUNT);
        _players.push_back(result);
        return result;
    }

    std::vector<CharacterCard*> getAvailableCharacters() {
        std::vector<CharacterCard*> result;
        for (const auto& pair : _characterPool)
            if (pair.second)
                result.push_back(pair.first);
        return result;
    }

    CharacterCard* getRandomAvailableCharacter() {
        auto characters = this->getAvailableCharacters();
        int top = characters.size();
        int i = rand() % top;
        auto result = characters[i];
        for (auto& pair : _characterPool) {
            if (pair.first == result) {
                pair.second = false;
                break;
            }
        }
        return result;
    }

    void shuffleLootDeck() {
        std::shuffle(std::begin(_lootDeck), std::end(_lootDeck), rng);
    }

    void shuffleTreasureDeck() {
        std::shuffle(std::begin(_treasureDeck), std::end(_treasureDeck), rng);
    }

    void createLootDeck(std::vector<std::pair<LootCard*, int>> pairs) {
        for (const auto& pair : pairs)
            for (int i = 0; i < pair.second; i++)
                this->_lootDeck.push_back(pair.first);
        this->shuffleLootDeck();
    }

    void createTreasureDeck(std::vector<TrinketCard*> cards) {
        for (const auto& card : cards)
            this->_treasureDeck.push_back(card);            
        this->shuffleTreasureDeck();
    }

    void start() {
        std::cout << "\nThe game starts\n\n";
        // setup script
        this->execScript("function _startTurnLoot(host)\n\tlocal owner = getOwner(host)\nlootCards(host, owner[\"id\"], owner[\"startTurnLootAmount\"])\nend");
        // give starting hands
        for (auto& p : _players) {
            auto cards = this->getTopLootCards(STARTING_LOOT_AMOUNT);
            p->addLootCards(cards);
        }
        // std::cout << "Gave the starting hands" << std::endl;
        // std::cout << "Loot deck:" << std::endl;
        // for (const auto& card : _lootDeck)
        //     std::cout << "\t" << card->name() << std::endl;
        // std::cout << "Treasure deck:" << std::endl;
        // for (const auto& card : _treasureDeck)
        //     std::cout << "\t" << card->name() << std::endl;
        // std::cout << "Players:" << std::endl;
        // for (const auto& p : _players)
        //     p->print();

        // this->_currentI = rand() % _players.size();
        this->_currentI = 0;

        this->_priorityI = this->_currentI;
        this->_running = true;

        int c = 10;
        while (this->_running) {
            this->calcNext();
            this->turn();
            this->passTurn();
            c--;
            if (!c) break;
        }   
    }

    void passTurn() {
        this->_currentI = this->_nextI;
    }

    void calcNext() {
        this->_nextI = (this->_currentI + 1) % _players.size();
    }

    void currentPlayerLoot() {
        this->pushToStack(StackEffect(
            "_startTurnLoot", 
            _activePlayer, 
            nullptr
        ));
    }

    void turn() {
        this->log("TURN " + std::to_string(++this->_turnCounter) + " - ");
        this->_activePlayer = this->_players[this->_currentI];
        this->log(this->_activePlayer->name() + " starts their turn");

        // recharge all of the items and character card of the active player
        this->_activePlayer->recharge();

        // apply all <start of turn> triggers, then resolve stack
        this->applyTriggers(TURN_START_TRIGGER);
        this->resolveStack();

        // add loot 1 to stack, then resolve stack
        this->currentPlayerLoot();
        this->resolveStack();

        // main step
        this->log(_activePlayer->name() + "'s main phase");
        string response = "";
        while ((response = this->_activePlayer->promptAction()) != ACTION_PASS) {
            std::cout << "\t" << _activePlayer->name() << ": " << response << std::endl;
            this->executePlayerAction(_activePlayer, response);
            this->resolveStack();
        }
        this->log("End of " + this->_activePlayer->name() + "'s turn");

        // end of turn
        //  TODO prompt the player to choose the order of triggers
        this->applyTriggers(TURN_END_TRIGGER);
        this->pushEOTDeferredTriggers();
        this->resolveStack();

        // all silent end of turn effects
        this->execEOTDefers();
    }

    void executePlayerAction(Player* player, string action) {
        auto split = str::split(action, " ");
        if (!this->_actionMap.count(split[0])) throw std::runtime_error("don't have a handler for " + split[0] + " action in match");
        this->_actionMap[split[0]](player, split);
        // this->pushToStack(StackEffect(
        //     "_customAction",
        //     player,
        //     nullptr
        // ));
    }

    void applyTriggers(string triggerType) {
        // first the monsters

        // then the items, starting with the current player
        //  TODO prompt the player to order them
        for (int i = 0; i < _players.size(); i++) {
            auto player = _players[(_currentI + i) % _players.size()];
            auto board = player->board();
            for (const auto& w : board) {
                auto card = (TrinketCard*)w->card();
                if (!card->hasTrigger(triggerType)) continue;
                // std::cout << "Card " << card->name() << "[" << w->id() << "] has a " << triggerType << " trigger" << std::endl;
                auto pair = card->getTriggerWhen(triggerType);
                auto checkFuncName = pair.first;
                if (!this->execCheck(checkFuncName)) continue;
                this->log(card->name() + " is triggered.");
                this->pushToStack(StackEffect(pair.second, player, w));
            }
        }
    }

    void pushToStack(const StackEffect& effect) {
        this->_stack.push_back(effect);
    }

    void resolveStack() {
        // while the stack isn't empty
        while (!this->_stack.empty()) {
            this->resolveTop();
        }
    }

    void resolveTop() {
        auto effect = this->_stack.back();
        // pass the priority of the owner of the effect
        // this->_priorityI = -1;
        // for (int i = 0; i < _players.size(); i++) {
        //     if (_players[i] == effect.player) {
        //         this->_priorityI = i;
        //         break;
        //     }
        // }
        this->_priorityI = this->_currentI;
        const auto last = this->_currentI;
        // if (this->_priorityI == -1) throw std::runtime_error("Unknown player " + effect.player->name());
        do {
            // prompt action
            auto response = this->promptPlayerWithPriority();
            if (response == ACTION_PASS) {
                // player passes priority
                this->log(this->_players[this->_priorityI]->name() + " passes priority");
                this->_priorityI = (this->_priorityI + 1) % _players.size();
                continue;
            }
            // player put something on stack
            return;
        } while (last != this->_priorityI);
        // resolve the ability
        this->execFunc(effect.funcName);
        this->_stack.pop_back();
    }

    string promptPlayerWithPriority() {
        auto player = this->_players[this->_priorityI];
        this->log("Player " + player->name() + " takes priority");
        return player->promptAction();
    }

    void log(string message) {
        std::cout << " - " << message << std::endl;
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    MatchState getState() {
        MatchState result;
        for (const auto& p : _players) 
            result.boards.push_back(p->getState());
        for (auto& si : _stack)
            result.stack.push_back(si.getState());
        result.currentI = _currentI;
        result.priorityI = _priorityI;
        result.lootDeckCount = _lootDeck.size();
        result.treasureDeckCount = _treasureDeck.size();
        for (const auto& c : _lootDiscard)
            result.lootDiscard.push_back(c->name());
        return result;
    }

    int newCardID() {
        return ++_lastID;
    }
};