#include "common.hpp"

#include "game/core.hpp"

const string REPLAYS_DIR = "../replays";

//  TODO

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
        auto result = _actions.front();
        _actions.pop();
        return result;
    }

    void update(MatchState& state) {}
    void updateEndMatch(MatchState& state, int winnerID) {}
    string promptAction(MatchState& state) { return popFirst(); }
    string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) { return popFirst(); }
    string promptSimpleResponse(MatchState& state, string text, vector<string> choices) { return popFirst(); }
    string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount) { return popFirst(); }
};

int main(int argc, char* argv[]) {
    
    // the first argument is always the path to the game
    if (argc == 1) {
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
            match->addPlayer(new ScriptedPlayer(
                name,
                match->getRandomAvailableCharacter(),
                pcount,
                actions
            ));
        }
        match->start();
        delete match;
        return 0;
    }
    if (argc == 4) {
        auto match = game.createMatch(1658236848);
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
                pcount,
                &server,
                allCards,
                conn
                // fs::readFile(fs::join(path, value["script"]).c_str())
            );
            match->addPlayer(p);
        }
        for (int i = 0; i < botC; i++) {
            pcount++;
            match->addPlayer(new BotPlayer(
                "player " + std::to_string(pcount),
                match->getRandomAvailableCharacter(),
                pcount,
                fs::readFile("bots/random.lua")
            ));
        }
        match->start();
        delete match;
        return 0;
    }
    std::cout << "Invalid number of arguments" << std::endl;
    
    return 0;
}