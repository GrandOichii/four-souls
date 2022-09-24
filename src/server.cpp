#include "common.hpp"

#include "window.hpp"

#include "game/core.hpp"

const string REPLAYS_DIR = "../replays";

class Server : public server_interface<PollType> {
private:
    int _connections = 0;
    int _maxConnections;

public:
    Server(uint16_t port, int maxConnections) :
        server_interface<PollType>(port),
        _maxConnections(maxConnections)
    {}

    tsqueue<owned_message<PollType>>& Incoming() {
        return m_qMessagesIn;
    }

    std::deque<std::shared_ptr<connection<PollType>>>& GetClients() {
        return m_deqConnections;
    }

    void WaitForMessages() {
        this->Update(1, true);
    }

    std::shared_ptr<connection<PollType>>& LastClient() {
        return _lastClient;
    }

    message<PollType>& LastMessage() {
        return _lastMsg;
    }

    int Connections() {
        return _connections;
    }

protected:
    std::shared_ptr<connection<PollType>> _lastClient;
    message<PollType> _lastMsg;

    virtual bool OnClientConnect(std::shared_ptr<connection<PollType>> client)
    {
        // std::cout << "Client connected, id: " << client->GetID() << std::endl;
        if (_maxConnections == _connections) return false;
        _connections++;
        std::cout << "NEW PLAYER " << _connections << "\t" << _maxConnections << std::endl;
        return true;
    }

    // Called when a client appears to have disconnected
    virtual void OnClientDisconnect(std::shared_ptr<connection<PollType>> client)
    {
        std::cout << "Client with id " << client->GetID() << " disconnected" << std::endl;
    }

    // Called when a message arrives
    virtual void OnMessage(std::shared_ptr<connection<PollType>> client, message<PollType> &msg)
    {
        // fix bug on too many characters
        _lastClient = client;
        _lastMsg = msg;
    }
};

static nlohmann::json cardToJson(Card* card) {
    auto result = nlohmann::json::object();
    result["name"] = card->name();
    result["key"] = card->key();
    result["text"] = card->text();
    return result;
}

static nlohmann::json cardsToJson(vector<Card*>& _allCards) {
    auto result = nlohmann::json::array();
    for (const auto& card : _allCards)
        result.push_back(cardToJson(card));
    return result;
}

class ConnectedPlayer : public Player {
private:
    Server* _server;
    std::shared_ptr<connection<PollType>> _conn;
public:
    ConnectedPlayer(std::string name, CharacterCard* card, int id, Server* server, vector<Card*>& _allCards, std::shared_ptr<connection<PollType>> conn) :
        Player(name, card, id),
        _server(server),
        _conn(conn)
    {
        message<PollType> msg;
        msg.header.id = PollType::Setup;
        nlohmann::json j;
        j["id"] = id;
        j["cards"] = cardsToJson(_allCards);

        string s = j.dump();
        msg << s;
        _server->MessageClient(_conn, msg);
    }

    ~ConnectedPlayer() {

    }

    string getResponse(MatchState& state) {
        message<PollType> msg;
        msg.header.id = PollType::GetAction;
        string s = state.toJson().dump();
        msg << s;
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }

    void update(MatchState& state) {
        message<PollType> msg;
        msg.header.id = PollType::Update;
        string s = state.toJson().dump();
        msg << s;
        _server->MessageClient(_conn, msg);   
    }

    void updateEndMatch(MatchState& state, int winnerID) {
        message<PollType> msg;
        msg.header.id = PollType::UpdateWinner;
        auto j = state.toJson();
        j["winnerID"] = winnerID;
        string s = j.dump();
        msg << s;
        _server->MessageClient(_conn, msg);   
    }

    string promptAction(MatchState& state) { 
        message<PollType> msg;
        msg.header.id = PollType::GetAction;
        string s = state.toJson().dump();
        msg << s;
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }

    string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) { 
        message<PollType> msg;
        msg.header.id = PollType::Prompt;
        auto j = state.toJson();
        j["prompt"] = nlohmann::json::object();
        j["prompt"]["text"] = text;
        j["prompt"]["choiceType"] = choiceType;
        j["prompt"]["choices"] = nlohmann::json::array();
        for (const auto& c : choices)
            j["prompt"]["choices"].push_back(c);
        string s = j.dump();
        msg << s;
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }

    string promptSimpleResponse(MatchState& state, string text, vector<string> choices) {
        message<PollType> msg;
        msg.header.id = PollType::SimplePrompt;
        auto j = state.toJson();
        j["prompt"] = nlohmann::json::object();
        j["prompt"]["text"] = text;
        j["prompt"]["choices"] = nlohmann::json::array();
        for (const auto& c : choices)
            j["prompt"]["choices"].push_back(c);
        string s = j.dump();
        msg << s;
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }

    string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount){ 
        message<PollType> msg;
        msg.header.id = PollType::ChooseCards;
        auto j = state.toJson();
        j["prompt"] = nlohmann::json::object();
        j["prompt"]["text"] = text;
        j["prompt"]["targetID"] = targetID;
        j["prompt"]["amount"] = amount;
        string s = j.dump();
        msg << s;
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }
};

class ScriptedPlayer : public Player {
private:
    std::queue<string> _actions;
public:
    ScriptedPlayer(string name, CharacterCard* card, int id, std::queue<string> actions) :
        Player(name, card, id),
        _actions(actions) {}

    string popFirst() {
        if (!_actions.size()) {
            throw std::runtime_error("scripted player ran out of actions");
            return "";
        }
        auto result = _actions.front();
        // name();
        // std::cout << result << std::endl;
        // std::cout << "PLAYER " << name() << " RETURNS " << result << std::endl;
        _actions.pop();
        return result;
    }

    virtual void update(MatchState& state) {}
    virtual void updateEndMatch(MatchState& state, int winnerID) {}
    virtual string promptAction(MatchState& state) { return popFirst(); }
    virtual string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) { return popFirst(); }
    virtual string promptSimpleResponse(MatchState& state, string text, vector<string> choices) { return popFirst(); }
    virtual string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount) { return popFirst(); }
};

class ObservingScriptedPlayer : public ScriptedPlayer {
private:
    Window* _win = nullptr;

public:
    ObservingScriptedPlayer(string name, CharacterCard* card, int id, std::queue<string> actions, vector<Card*> cards) :
        ScriptedPlayer(name, card, id, actions)
    {
        _win = new Window("player replay", "assets", false);
        for (const auto& card : cards)
            _win->assets()->createCard(card->name(), card->text());
    }

    ~ObservingScriptedPlayer() {
        delete _win;
    }
    
    void draw(MatchState& state) {
        _win->clear();
        _win->draw(state);
        _win->flush();
    }

    void update(MatchState& state) override {
        this->draw(state);
    }

    void updateEndMatch(MatchState& state, int winnerID) override {
        this->draw(state);
    }

    string promptAction(MatchState& state) override { 
        auto result = ScriptedPlayer::promptAction(state);
        this->draw(state);
        return result; 
    }

    string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) override { 
        auto result = ScriptedPlayer::promptResponse(state, text, choiceType, choices);
        this->draw(state);
        return result; 
    }

    string promptSimpleResponse(MatchState& state, string text, vector<string> choices) override { 
        auto result = ScriptedPlayer::promptSimpleResponse(state, text, choices);
        this->draw(state);
        return result;
    }

    string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount) override {
        auto result = ScriptedPlayer::promptChooseCardsInHand(state, text, targetID, amount);
        this->draw(state);
        return result;
    }
};

const char* BOT_SCRIPT_PATH = "bots/experimental.lua";

int main(int argc, char* argv[]) {
    // the first argument is always the path to the game
    if (argc == 1 || argc > 5) {
        std::cout << "Invalid number of arguments" << std::endl;
        return 0;
    }
    Game game(argv[1]);
    auto allCards = game.getAllCards();

    if (argc == 3) {
        // the only argument is the path to the replay file
        auto replayPath = string(argv[2]);
        auto j = fs::readJS(replayPath);
        auto match = game.createMatch(j["seed"]);
            
        // pray to god that the first player will be first
        int pcount = 0;
        for (const auto& [name, value] : j["actions"].items()) {
            std::queue<string> actions;
            for (const auto& [n, v] : value.items()) actions.push(v);
            pcount++;
            if (pcount == 1) {
                // match->addPlayer(new ObservingScriptedPlayer(
                //     name,
                //     match->getRandomAvailableCharacter(),
                //     match->newCardID(),
                //     actions,
                //     allCards
                // ));
                
                match->addPlayer(new ScriptedPlayer(
                    name,
                    match->getRandomAvailableCharacter(),
                    match->newCardID(),
                    actions
                ));
                continue;
            }
            // match->addPlayer(new BotPlayer(
            //     name,
            //     match->getRandomAvailableCharacter(),
            //     match->newCardID(),
            //     fs::readFile(BOT_SCRIPT_PATH)
            // ));

            match->addPlayer(new ScriptedPlayer(
                name,
                match->getRandomAvailableCharacter(),
                match->newCardID(),
                actions
            ));
        }
        try {
            match->start();
        } catch (std::runtime_error& ex) {
            match->dumpStacks();
            std::cout << ex.what() << std::endl;
            delete match;
            return 1;
        }
        delete match;
        return 0;
    }
    auto match = game.createMatch();
    if (argc == 5) {
        delete match;
        match = game.createMatch(atoi(argv[4]));
    }
    // first is amount of bot players, second is amount of real players
    auto botC = atoi(argv[2]);
    auto playerC = atoi(argv[3]);
    Server server(9090, playerC);
    server.Start();
    while (server.Connections() != playerC) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "Waiting for players.." << std::endl;            
    }
    int pcount = 0;
    for (auto& conn : server.GetClients()) {
        pcount++;
        auto p = new ConnectedPlayer(
            "player " + std::to_string(pcount),
            match->getRandomAvailableCharacter(),
            match->newCardID(),
            &server,
            allCards,
            conn
            // fs::readFile(fs::join(path, value["script"]).c_str())
        );
        match->addPlayer(p);
    }
    for (int i = 0; i < botC; i++) {
        pcount++;
        if (i == 1) {
            match->addPlayer(new BotPlayer(
                "player " + std::to_string(pcount),
                match->getRandomAvailableCharacter(),
                match->newCardID(),
                fs::readFile("bots/experimental.lua")
            ));
            continue;
        }
        match->addPlayer(new BotPlayer(
            "player " + std::to_string(pcount),
            match->getRandomAvailableCharacter(),
            match->newCardID(),
            fs::readFile(BOT_SCRIPT_PATH)
        ));
    }
    try {
        match->start();
    } catch (std::runtime_error& ex) {
        match->dumpStacks();
        std::cout << ex.what() << std::endl;
        delete match;
        return 1;
    }
    delete match;
    
    return 0;
}