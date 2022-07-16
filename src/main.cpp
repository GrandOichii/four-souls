#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>  

#include "network/olc_network.hpp"
#include "util.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include "game/core.hpp"
#include "game/cards.hpp"
#include "game/match.hpp"
#include "game/types.hpp"

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

using std::cout;
using std::endl;
using std::string;
using std::vector;

std::pair<int, int> getSize(SDL_Texture *texture) {
    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return std::make_pair(size.x, size.y);
}

int CONNECTED_PLAYERS;
int BOTS;

class Font {
private:
    SDL_Renderer* _ren;
    TTF_Font *_font;
public:
    Font(const char* path, int fontSize, SDL_Renderer* ren) : 
        _ren(ren) 
    {
        if (TTF_Init() != 0){
            std::cout << "TTF_init Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("failed initializing font");
        }
        this->_font = TTF_OpenFont(path, fontSize);
        if (_font == nullptr){
            std::cout << "TTF_OpenFont Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("failed opening font file " + std::string(path));
        }       
    }

    ~Font() {
        TTF_CloseFont(_font);
    }

    SDL_Texture * get(std::string message, SDL_Color color) {
        SDL_Surface *surf = TTF_RenderText_Blended(_font, message.c_str(), color);
        if (surf == nullptr){
            TTF_CloseFont(_font);
            std::cout << "TTF_RenderText Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("failed rendering message " + message);
        }
        SDL_Texture *result = SDL_CreateTextureFromSurface(this->_ren, surf);
        if (result == nullptr){
            std::cout << "CreateTexture Error: " << SDL_GetError() << std::endl;

            throw std::runtime_error("failed creating texture of message " + message);
        }
        SDL_FreeSurface(surf);
        return result;
    }

    SDL_Texture * get(std::string message) {
        return this->get(message, SDL_Color{255, 255, 255, 255});
    }
};

class AssetsManager {
private:
    SDL_Renderer* _ren = nullptr;

    std::map<string, SDL_Texture*> _textureMap;
    std::map<string, SDL_Texture*> _cardBacks;

    std::map<int, Font*> _fontMap;
public:
    AssetsManager(SDL_Renderer* ren, string fontPath) :
        _ren(ren) 
    {
        std::vector<int> fontSizes = {24, 48};
        for (auto& fs : fontSizes)
            _fontMap[fs] = new Font(fontPath.c_str(), fs, _ren);
    }

    ~AssetsManager() {
        for (const auto& [key, value] : _textureMap) SDL_DestroyTexture(value);
        for (const auto& [key, value] : _cardBacks) SDL_DestroyTexture(value);
        for (const auto& [key, value] : _fontMap) delete value;
    }

    std::pair<int, int> cardSize() { return getSize(_textureMap.begin()->second); }

    void addCardBack(string cardType, string path) {
        auto tex = this->loadBMP(path);
        this->_cardBacks[cardType] = tex;
    }

    void addCard(Card* card) {
        auto tex = this->loadBMP(card->imagePath());
        auto name = card->name();
        this->_textureMap[name] = tex;
    }

    SDL_Texture* getCard(string cardName) {
        if (!_textureMap.count(cardName)) throw std::runtime_error("no image for card " + cardName);
        return _textureMap[cardName];
    }

    SDL_Texture* getCardBack(string cardType) {        
        if (!_cardBacks.count(cardType)) throw std::runtime_error("no image for card back " + cardType);
        return _cardBacks[cardType];
    }

    SDL_Texture *loadBMP(string path) {
        SDL_Surface *bmp = SDL_LoadBMP(path.c_str());
        if (bmp == nullptr){
            std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("");
        }
        SDL_Texture *result = SDL_CreateTextureFromSurface(_ren, bmp);
        SDL_FreeSurface(bmp);
        if (result == nullptr){
            std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("");
        }
        return result;
    }

    SDL_Texture* getMessage(string message, SDL_Color color, int fontSize) {
        if (!_fontMap.count(fontSize)) throw std::runtime_error("no font with size " + std::to_string(fontSize));
        return _fontMap[fontSize]->get(message, color);
    }
};

enum PollType : uint8_t {
    // client - server
    ServerMessage,

    // server - client
    Update,
    GetAction,
    Prompt,
    SimplePrompt,
    ChooseCards
};

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

class ConnectedPlayer : public Player {
private:
    Server* _server;
    std::shared_ptr<connection<PollType>> _conn;
public:
    ConnectedPlayer(std::string name, CharacterCard* card, int id, Server* server, std::shared_ptr<connection<PollType>> conn) :
        Player(name, card, id),
        _server(server),
        _conn(conn)
    {}

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

class GameWrapper {
private:
    Server* _server;
    bool _fullscreen;
    string _title;

    Game* _game = nullptr;
    Match* _match = nullptr;
    AssetsManager* _assets = nullptr;

    SDL_Window *_win = nullptr;
    SDL_Renderer *_ren = nullptr;
    SDL_Event* _event = new SDL_Event();
    bool _running = false;

    int _sideBoardX;

    int _wWidth;
    int _wHeight;
    int _stackX;
    int _boardWidth;
    int _boardHeight;

    int _lootDeckX;
    int _lootDeckY;
    int _lootDiscardX;

    int _treasureDeckX;
    int _treasureDeckY;
    int _treasureDiscardX;

    int _monsterDeckX;
    int _monsterDeckY;
    int _monsterDiscardX;

    std::thread _gameMatchThread;

    int _playerSpaces[4][2];
    std::pair<int, int> _cardSize;

    int _lastLootDeckCount = 0;
    SDL_Texture* _lastLootDeckCountTex = nullptr;
    int _lastLootDiscardCount = 0;
    SDL_Texture* _lastLootDiscardCountTex = nullptr;
    int _lastTreasureDeckCount = 0;
    SDL_Texture* _lastTreasureDeckCountTex = nullptr;
    int _lastTreasureDiscardCount = 0;
    SDL_Texture* _lastTreasureDiscardCountTex = nullptr;
    int _lastMonsterDeckCount = 0;
    SDL_Texture* _lastMonsterDeckCountTex = nullptr;
    int _lastMonsterDiscardCount = 0;
    SDL_Texture* _lastMonsterDiscardCountTex = nullptr;
public:
    GameWrapper(string title, string path, string playersPath, bool fullscreen) :
        _title(title),
        _fullscreen(fullscreen)
    {
        this->_game = new Game("game");
        this->_match = _game->createMatch();
        // start server
        _server = new Server(9090, CONNECTED_PLAYERS);
        _server->Start();

        // add players
        this->addPlayers();

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_StartTextInput(); 
        this->_win = SDL_CreateWindow(this->_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);
        if (this->_win == nullptr){
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetWindowFullscreen(_win, this->_fullscreen);
        SDL_GetWindowSize(this->_win, &this->_wWidth, &this->_wHeight);
        this->_ren = SDL_CreateRenderer(this->_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (this->_ren == nullptr){
            std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return;
        }

        // calculate player spaces
        this->_sideBoardX = 200;
        auto stackWidth = 150;
        this->_stackX = this->_wWidth - stackWidth;

        this->_boardWidth = this->_stackX - 1 - this->_sideBoardX;
        this->_boardHeight = this->_wHeight;

        this->_playerSpaces[0][0] = this->_sideBoardX;
        this->_playerSpaces[0][1] = 0;

        this->_playerSpaces[1][0] = this->_sideBoardX + _boardWidth / 2+1;
        this->_playerSpaces[1][1] = 0;

        this->_playerSpaces[2][0] = this->_sideBoardX + _boardWidth / 2+1;
        this->_playerSpaces[2][1] = _boardHeight / 2+1;

        this->_playerSpaces[3][0] = this->_sideBoardX;
        this->_playerSpaces[3][1] = _boardHeight / 2+1;

        this->_assets = new AssetsManager(this->_ren, "font/font.ttf");
        auto allCards = this->_game->getAllCards();
        for (const auto& c : allCards)
            this->_assets->addCard(c);
        this->_cardSize = this->_assets->cardSize();
        this->_assets->addCardBack(CARD_TYPE_LOOT, _game->lootCardBackPath());
        this->_assets->addCardBack(CARD_TYPE_TREASURE, _game->treasureCardBackPath());
        this->_assets->addCardBack(CARD_TYPE_MONSTER, _game->monsterCardBackPath());

        _lastLootDiscardCountTex = this->_assets->getMessage("0", SDL_Color{255, 255, 255, 0}, 24);
        _lastTreasureDiscardCountTex = this->_assets->getMessage("0", SDL_Color{255, 255, 255, 0}, 24);
        _lastMonsterDiscardCountTex = this->_assets->getMessage("0", SDL_Color{255, 255, 255, 0}, 24);

        const int deckCount = 3;
        int between = 3;
        int startY = (this->_wHeight - deckCount * (_cardSize.second+between)) / 2;

        int discardX = between;
        int mainX = discardX + _cardSize.first + between;

        this->_treasureDeckX = mainX;
        this->_treasureDiscardX = discardX;
        this->_treasureDeckY = startY;

        this->_lootDeckX = mainX;
        this->_lootDiscardX = discardX;
        this->_lootDeckY = startY + _cardSize.second + between;

        this->_monsterDeckX = mainX;
        this->_monsterDiscardX = discardX;
        this->_monsterDeckY = startY + (_cardSize.second + between) * 2;
    }

    ~GameWrapper() {
        delete _game;
        delete _match;
        delete _assets;
        delete _server;

        SDL_DestroyTexture(_lastLootDeckCountTex);
        SDL_DestroyTexture(_lastLootDiscardCountTex);
        SDL_DestroyTexture(_lastTreasureDeckCountTex);
        SDL_DestroyTexture(_lastTreasureDiscardCountTex);
        SDL_DestroyTexture(_lastMonsterDeckCountTex);
        SDL_DestroyTexture(_lastMonsterDiscardCountTex);
    }

    void addPlayers() {
        while (_server->Connections() != CONNECTED_PLAYERS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            std::cout << "Waiting for players.." << std::endl;            
        }
        
        int pcount = 0;
        for (auto& conn : _server->GetClients()) {
            pcount++;
            auto p = new ConnectedPlayer(
                "player " + std::to_string(pcount),
                _match->getRandomAvailableCharacter(),
                pcount,
                _server,
                conn
                // fs::readFile(fs::join(path, value["script"]).c_str())
            );
            _match->addPlayer(p);
        }
        for (int i = 0; i < BOTS; i++) {
            pcount++;
            _match->addPlayer(new BotPlayer(
                "player " + std::to_string(pcount),
                _match->getRandomAvailableCharacter(),
                pcount,
                fs::readFile("game/bots/random.lua")
            ));
            
        }
    }

    void start() {
        this->setup();
        this->_running = true;
        while (this->_running) {
            // handle events
            while (SDL_PollEvent(this->_event)) {
                switch (_event->type) {
                case SDL_QUIT:
                    this->_running = false;
                    break;
                case SDL_KEYDOWN:
                    this->handleKey(_event->key.keysym.sym);
                }
            }
            this->clear();
            this->draw();
            this->flush();
        }
        this->_gameMatchThread.detach();
    }

    void setup() {
        this->_gameMatchThread = std::thread(&Match::start, this->_match);
    }

    void handleKey(int key) {
        switch (key) {
        case SDLK_ESCAPE:
            this->_running = false;
            return;
        case 'f':
            this->toggleFullscreen();
            return;
        }
    }

    void toggleFullscreen() {
        this->_fullscreen = !this->_fullscreen;
        SDL_SetWindowFullscreen(_win, this->_fullscreen);
        SDL_GetWindowSize(this->_win, &this->_wWidth, &this->_wHeight);
    }

    void clear() {
        SDL_RenderClear(_ren);
    }

    void drawCard(const CardState& card, int x, int y) {
        this->drawTexture(this->_assets->getCard(card.cardName), x, y, card.active ? 0 : 90);
        auto tex = _assets->getMessage("["+std::to_string(card.id)+"]", SDL_Color{255, 0, 255, 0}, 24);
        drawTexture(tex, x + 2, y + 2);
        SDL_DestroyTexture(tex);
        if (card.counters) {
            tex = _assets->getMessage(std::to_string(card.counters), SDL_Color{0, 0, 255, 0}, 24);
            drawTexture(tex, x + 2, y + 2 + 24);
            SDL_DestroyTexture(tex);
        }
    }

    void drawCard(string cardName, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCard(cardName), x, y, active ? 0 : 90);
    }

    void drawCardBack(string cardType, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCardBack(cardType), x, y, active ? 0 : 90);
    }

    void draw() {
        // return;
        // draw player separators
        this->drawLine(
            this->_sideBoardX + this->_boardWidth / 2, 
            0, 
            this->_sideBoardX + this->_boardWidth / 2, 
            this->_boardHeight-1, 
        SDL_Color{255, 0, 0, 0});
        this->drawLine(
            this->_sideBoardX, 
            this->_boardHeight / 2, 
            this->_sideBoardX + this->_boardWidth-1, 
            this->_boardHeight / 2, 
        SDL_Color{255, 0, 0, 0});

        // get game state
        auto state = this->_match->getState();
        // draw boards
        this->drawBoards(state);
        // draw decks
        this->drawSideBoard(state);
        // draw stack
        this->drawStack(state);
    }

    void drawSideBoard(const MatchState& state) {
        this->drawLoot(state);
        this->drawTreasure(state);
        this->drawMonsters(state);
    }

    void drawMonsters(const MatchState& state) {
        // draw monster deck
        auto count = state.monsterDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_MONSTER, true, _monsterDeckX, _monsterDeckY);
        // _lastLootDeckCount
        if (count != _lastMonsterDeckCount) {
            _lastMonsterDeckCount = count;
            SDL_DestroyTexture(_lastMonsterDeckCountTex);
            _lastMonsterDeckCountTex = this->_assets->getMessage(std::to_string(_lastMonsterDeckCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastMonsterDeckCountTex, _monsterDeckX + 10, _monsterDeckY + 10);
        // draw monster discard
        count = state.monsterDiscard.size();
        if (count) {
            this->drawCard(*(state.monsterDiscard.end()-1), _monsterDiscardX, _monsterDeckY);
        }
        if (count != _lastMonsterDeckCount) {
            _lastMonsterDiscardCount = count;
            SDL_DestroyTexture(_lastMonsterDiscardCountTex);
            _lastMonsterDiscardCountTex = this->_assets->getMessage(std::to_string(_lastMonsterDiscardCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastMonsterDiscardCountTex, _monsterDiscardX + 10, _monsterDeckY + 10);
        // draw monster slots
        auto y = _monsterDeckY + _cardSize.second;
        for (const auto& card : state.monsters) {
            this->drawTexture(this->_assets->getCard(card.cardName),_monsterDiscardX + 20, y, -90);
            // auto tex = this->_assets->getCard(name);
            // this->drawTexture(tex, _monsterDiscardX + 20, y, -90);
            y += _cardSize.first + 3;
        }
    }

    void drawTreasure(const MatchState& state) {
        // draw treasure deck
        auto count = state.treasureDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_TREASURE, true, _treasureDeckX, _treasureDeckY);
        // _lastLootDeckCount
        if (count != _lastTreasureDeckCount) {
            _lastTreasureDeckCount = count;
            SDL_DestroyTexture(_lastTreasureDeckCountTex);
            _lastTreasureDeckCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDeckCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastTreasureDeckCountTex, _treasureDeckX + 10, _treasureDeckY + 10);
        // draw treasure discard
        count = state.treasureDiscard.size();
        // std::cout << "\t" << count << std::endl;
        if (count) {
            this->drawCard(*(state.treasureDiscard.end()-1), _treasureDiscardX, _treasureDeckY);
        }
        if (count != _lastTreasureDiscardCount) {
            _lastTreasureDiscardCount = count;
            SDL_DestroyTexture(_lastTreasureDiscardCountTex);
            _lastTreasureDiscardCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDiscardCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastTreasureDiscardCountTex, _treasureDiscardX + 10, _treasureDeckY + 10);
        // draw shop
        auto y = _treasureDeckY - _cardSize.second;
        for (const auto& card : state.shop) {
            this->drawTexture(this->_assets->getCard(card.cardName),_treasureDiscardX + 20, y, -90);
            // auto tex = this->_assets->getCard(name);
            // this->drawTexture(tex, _treasureDiscardX + 20, y, -90);
            y -= _cardSize.first + 3;
            // auto tex = this->drawCard(name, true);
        }
    }

    void drawLoot(const MatchState& state) {
        // draw loot deck
        auto count = state.lootDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_LOOT, true, _lootDeckX, _lootDeckY);
        if (count != _lastLootDeckCount) {
            _lastLootDeckCount = count;
            SDL_DestroyTexture(_lastLootDeckCountTex);
            _lastLootDeckCountTex = this->_assets->getMessage(std::to_string(_lastLootDeckCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastLootDeckCountTex, _lootDeckX + 10, _lootDeckY + 10);
        // draw loot discard
        count = state.lootDiscard.size();
        if (count) {
            this->drawCard(*(state.lootDiscard.end()-1), _lootDiscardX, _lootDeckY);
        }
        if (count != _lastLootDiscardCount) {
            _lastLootDiscardCount = count;
            SDL_DestroyTexture(_lastLootDiscardCountTex);
            _lastLootDiscardCountTex = this->_assets->getMessage(std::to_string(_lastLootDiscardCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastLootDiscardCountTex, _lootDiscardX + 10, _lootDeckY + 10);
    }

    void drawBoards(const MatchState& state) {
        for (int i = 0; i < state.boards.size(); i++) {
            auto space = state.boards[i];
            int pX = _playerSpaces[i][0];
            int pY = _playerSpaces[i][1];
            int lootX = pX + 200;
            int looyY = pY + 10;
            if (i == state.currentI)
                this->drawRect(pX, pY, _boardWidth/2, _boardHeight/2, SDL_Color{0, 150, 0, 0}, true);
            if (i == state.priorityI) {
                int width = _boardWidth/2;
                int height = _boardHeight/2;
                this->drawRect(pX, pY, width, height, SDL_Color{0, 255, 255, 0}, false);
                this->drawRect(pX+1, pY+1, width-2, height-2, SDL_Color{0, 255, 255, 0}, false);
            }
            
            pX += _cardSize.second - _cardSize.first;
            auto cCard = space.playerCard;

            this->drawCard(cCard, pX+10, pY+10);
            pY += 150;
            int betweenCards = 2;
            for (const auto& card : space.board) {
                this->drawCard(card, pX+10, pY);
                pX += _cardSize.second + betweenCards;
            }
            for (const auto& card : space.hand) {
                this->drawCard(card, lootX, looyY);
                lootX += _cardSize.first + betweenCards;
            }

            pX = _playerSpaces[i][0];
            pY = _playerSpaces[i][1];
            // draw coins
            auto tex = this->_assets->getMessage(std::to_string(space.coinCount), SDL_Color{255, 255, 51, 0}, 48);
            drawTexture(tex, pX + 10, pY + 10);
            SDL_DestroyTexture(tex);
            // draw health
            // tex = this->_assets->getMessage(std::to_string(space.health) + "/" + std::to_string(space.maxHealth), SDL_Color{255, 0, 0, 0}, 48);
            tex = this->_assets->getMessage(std::to_string(space.health), SDL_Color{255, 0, 0, 0}, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 1);
            auto width = getSize(tex).first;
            SDL_DestroyTexture(tex);
            // draw blue health
            if (space.blueHealth) {
                tex = this->_assets->getMessage("+" + std::to_string(space.blueHealth), SDL_Color{0, 0, 255, 0}, 48);
                drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
                width += getSize(tex).first;
                SDL_DestroyTexture(tex);
            }
            // draw max health
            tex = this->_assets->getMessage("/" + std::to_string(space.maxHealth), SDL_Color{255, 0, 0, 0}, 48);
            drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
            SDL_DestroyTexture(tex);
            // draw attack
            tex = this->_assets->getMessage(std::to_string(space.attack), SDL_Color{169, 169, 169, 0}, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 2);
            SDL_DestroyTexture(tex);
            // draw souls
            tex = this->_assets->getMessage(std::to_string(space.soulCount), SDL_Color{255, 255, 255, 0}, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 3);
            SDL_DestroyTexture(tex);


        }
    }

    void drawStack(const MatchState& state) {
        this->drawRect(_stackX, 0, _wWidth - _stackX, _wHeight, SDL_Color{169, 169, 169, 0}, true);
        int yOffset = 10;
        int x = _stackX + 10;
        int y = yOffset;
        for (const auto& si : state.stack) {
            if (si.isCard) {
                this->drawCard(si.card, x, y);
            }
            else {
                drawRect(x, y, _cardSize.first, _cardSize.second, SDL_Color{0, 0, 255, 0}, true);
                auto lines = str::split(si.message, "\n");
                for (int i = 0; i < lines.size(); i++) {
                    auto tex = _assets->getMessage(lines[i], SDL_Color{200, 200, 100, 0}, 24);
                    drawTexture(tex, x + 4, y + 4 + 24 * i);
                    SDL_DestroyTexture(tex);

                }
            }
            y += _cardSize.second + yOffset;
        }
    }

    void flush() {
        SDL_RenderPresent(_ren);
    }

    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
        SDL_SetRenderDrawColor( _ren, color.r, color.g, color.b, 0 );
        SDL_RenderDrawLine(_ren, x1, y1, x2, y2);
        SDL_SetRenderDrawColor(_ren, 0, 0, 0, 0 );
    }

    void drawTexture(SDL_Texture *texture, int x, int y, int angle=0) {
        auto size = getSize(texture);
        SDL_Rect dstrect;
        dstrect.w = size.first;
        dstrect.h = size.second;
        dstrect.x = x;
        dstrect.y = y;
        SDL_RenderCopyEx( _ren, texture, nullptr, &dstrect, angle, NULL, SDL_FLIP_NONE );        
    }

    void drawRect(int x, int y, int width, int height, SDL_Color color, bool fill) {
        SDL_SetRenderDrawColor( _ren, color.r, color.g, color.b, 0 ); // last color.a?
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = width;
        r.h = height;
        if (fill)
            SDL_RenderFillRect(this->_ren, &r);
        else SDL_RenderDrawRect(this->_ren, &r);
        SDL_SetRenderDrawColor(this->_ren, 0, 0, 0, 0 );
    }
};

class Client : public client_interface<PollType> {
public:
    void ping_server()
    {
        // message<PollType> msg;
        // msg.header.id = int::ServerPing;
        // msg.header.name = user_name;
        // send(msg);
    }

    void message_all()
    {
        // message<PollType> msg;
        // msg.header.id = int::MessageAll;
        // msg.header.name = user_name;
        // send(msg);
    }

    void join_server()
    {
        // std::cout << "Input Your Name: ";
        // std::wcin.get(user_name.data(), user_name.size());
        // std::wcin.clear();
        // std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // message<PollType> msg;
        // msg.header.id = int::JoinServer;
        // msg.header.name = user_name;
        // send(msg);
    }

    void send_msg(std::wstring& __data)
    {
        // message<PollType> msg;
        // msg.header.id = int::PassString;
        // msg.header.name = user_name;
        // for (unsigned int i{}, j{}; j < __data.size(); ++i, ++j)
        //     msg.data[i] = __data[j];

        // send(msg);
    }

public:
    // std::array<wchar_t, 256> user_name{};
};

class ClientWrapper {
private:
    bool _waitingResponse = false;
    PollType _lastRequestType;

    vector<string> _simpleChoices;
    vector<SDL_Texture*> _simpleChoiceTextures;
    int _maxSimpleChoiceLength = 0;

    int _cardAmount = 0;
    vector<int> _chosenCardIDs;

    vector<int> _allowedCards;
    vector<int> _allowedPlayers;
    vector<int> _allowedStackMembers;

    SDL_Texture* _lastTextTex = nullptr;
    
    string _host;
    Client* _c;
    bool _hasState = false;
    MatchState _state;
    bool _fullscreen;
    string _title;
    bool _mouseLock = false;

    Game* _game = nullptr;
    AssetsManager* _assets = nullptr;

    SDL_Window* _win = nullptr;
    SDL_Renderer* _ren = nullptr;
    SDL_Event* _event = new SDL_Event();
    bool _running = false;

    int _sideBoardX;

    int _boardStart;
    int _wWidth;
    int _wHeight;
    int _stackX;
    int _boardWidth;
    int _boardHeight;

    int _lootDeckX;
    int _lootDeckY;
    int _lootDiscardX;

    int _treasureDeckX;
    int _treasureDeckY;
    int _treasureDiscardX;

    int _monsterDeckX;
    int _monsterDeckY;
    int _monsterDiscardX;

    int _playerSpaces[4][2];
    std::pair<int, int> _cardSize;

    int _lastLootDeckCount = 0;
    SDL_Texture* _lastLootDeckCountTex = nullptr;
    int _lastLootDiscardCount = 0;
    SDL_Texture* _lastLootDiscardCountTex = nullptr;
    int _lastTreasureDeckCount = 0;
    SDL_Texture* _lastTreasureDeckCountTex = nullptr;
    int _lastTreasureDiscardCount = 0;
    SDL_Texture* _lastTreasureDiscardCountTex = nullptr;
    int _lastMonsterDeckCount = 0;
    SDL_Texture* _lastMonsterDeckCountTex = nullptr;
    int _lastMonsterDiscardCount = 0;
    SDL_Texture* _lastMonsterDiscardCountTex = nullptr;
public:
    ClientWrapper(string host, string title, string path, bool fullscreen) :
        _host(host),
        _title(title),
        _fullscreen(fullscreen)
    {
        // _allowedCards.clear();
        // _allowedPlayers.clear();
        // _allowedStackMembers.clear();
        // _chosenCardIDs.clear();
        // _simpleChoices.clear();
        clearSimpleChoiceTextures();
        this->_game = new Game("game");

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_StartTextInput();
        this->_win = SDL_CreateWindow(this->_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);
        if (this->_win == nullptr) {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetWindowFullscreen(_win, this->_fullscreen);
        SDL_GetWindowSize(this->_win, &this->_wWidth, &this->_wHeight);
        this->_ren = SDL_CreateRenderer(this->_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (this->_ren == nullptr) {
            std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return;
        }

        // calculate player spaces
        this->_sideBoardX = 200;
        auto stackWidth = 150;
        this->_stackX = this->_wWidth - stackWidth;

        this->_boardStart = 24;

        this->_boardWidth = this->_stackX - 1 - this->_sideBoardX;
        this->_boardHeight = this->_wHeight - _boardStart;

        this->_playerSpaces[0][0] = this->_sideBoardX;
        this->_playerSpaces[0][1] = _boardStart + _boardHeight / 2 + 1;

        this->_playerSpaces[1][0] = this->_sideBoardX;
        this->_playerSpaces[1][1] = _boardStart;

        this->_playerSpaces[2][0] = this->_sideBoardX + _boardWidth / 2 + 1;
        this->_playerSpaces[2][1] = _boardStart;

        this->_playerSpaces[3][0] = this->_sideBoardX + _boardWidth / 2 + 1;
        this->_playerSpaces[3][1] = _boardStart + _boardHeight / 2 + 1;

        this->_assets = new AssetsManager(this->_ren, "font/font.ttf");
        auto allCards = this->_game->getAllCards();
        for (const auto& c : allCards)
            this->_assets->addCard(c);
        this->_cardSize = this->_assets->cardSize();
        this->_assets->addCardBack(CARD_TYPE_LOOT, _game->lootCardBackPath());
        this->_assets->addCardBack(CARD_TYPE_TREASURE, _game->treasureCardBackPath());
        this->_assets->addCardBack(CARD_TYPE_MONSTER, _game->monsterCardBackPath());

        _lastLootDiscardCountTex = this->_assets->getMessage("0", SDL_Color{ 255, 255, 255, 0 }, 24);
        _lastTreasureDiscardCountTex = this->_assets->getMessage("0", SDL_Color{ 255, 255, 255, 0 }, 24);
        _lastMonsterDiscardCountTex = this->_assets->getMessage("0", SDL_Color{ 255, 255, 255, 0 }, 24);

        const int deckCount = 3;
        int between = 3;
        int startY = (this->_wHeight - deckCount * (_cardSize.second + between)) / 2;

        int discardX = between;
        int mainX = discardX + _cardSize.first + between;

        this->_treasureDeckX = mainX;
        this->_treasureDiscardX = discardX;
        this->_treasureDeckY = startY;

        this->_lootDeckX = mainX;
        this->_lootDiscardX = discardX;
        this->_lootDeckY = startY + _cardSize.second + between;

        this->_monsterDeckX = mainX;
        this->_monsterDiscardX = discardX;
        this->_monsterDeckY = startY + (_cardSize.second + between) * 2;
    }

    ~ClientWrapper() {
        delete _game;
        delete _c;
        delete _assets;

        SDL_DestroyTexture(_lastTextTex);
        SDL_DestroyTexture(_lastLootDeckCountTex);
        SDL_DestroyTexture(_lastLootDiscardCountTex);
        SDL_DestroyTexture(_lastTreasureDeckCountTex);
        SDL_DestroyTexture(_lastTreasureDiscardCountTex);
        SDL_DestroyTexture(_lastMonsterDeckCountTex);
        SDL_DestroyTexture(_lastMonsterDiscardCountTex);
    }

    void getActionCalc() {
        auto me = _state.boards[_state.priorityI];
        // check character card
        if (me.playerCard.active) _allowedCards.push_back(me.playerCard.id);
        // check board
        for (const auto& value : me.board)
            if (value.active) _allowedCards.push_back(value.id);
        // check loot cards
        if (me.playableCount)
            for (const auto& value : me.hand)
                _allowedCards.push_back(value.id);
        // check treasure
        if (me.purchaseCount && me.coinCount >= me.treasurePrice && _state.stack.size() == 0)
            for (const auto& value : _state.shop)
                _allowedCards.push_back(value.id);
        // check monsters
        //  TODO
    }

    void clearSimpleChoiceTextures() {
        for (const auto& tex : _simpleChoiceTextures) SDL_DestroyTexture(tex);
        _simpleChoiceTextures.clear();
    }

    void promptCalc(nlohmann::json j) {
        std::vector<int> choices;
        for (const auto& [key, value] : j["prompt"]["choices"].items()) choices.push_back(value);
        // last text texture
        SDL_DestroyTexture(this->_lastTextTex);
        this->_lastTextTex = _assets->getMessage(j["prompt"]["text"], SDL_Color{255, 0, 0, 0}, _boardStart);
        auto lastChoiceType = j["prompt"]["choiceType"];
        if (lastChoiceType == PLAYER_TARGET) {
            _allowedPlayers = choices;
            return;
        }
        if (lastChoiceType == CARD_TARGET) {
            _allowedCards = choices;
            return;
        }
        if (lastChoiceType == STACK_MEMBER_TARGET) {
            _allowedStackMembers = choices;
            return;
        }
        //  TODO items in shop
        //  TODO monsters
    }

    void simplePromptCalc(nlohmann::json j) {
        SDL_DestroyTexture(this->_lastTextTex);
        this->_lastTextTex = _assets->getMessage(j["prompt"]["text"], SDL_Color{255, 0, 0, 0}, 24);
        _maxSimpleChoiceLength = 0;
        for (const auto& [key, value] : j["prompt"]["choices"].items()) {
            _simpleChoices.push_back(value);
            auto tex = _assets->getMessage(value, SDL_Color{0, 0, 0, 0}, 48);
            _simpleChoiceTextures.push_back(tex);
            auto size = getSize(tex);
            if (size.first > _maxSimpleChoiceLength) _maxSimpleChoiceLength = size.first;
        }
        _maxSimpleChoiceLength += 4;
    }

    void drawSimplePromptBox() {
        if (!_simpleChoices.size()) return;
        // std::cout << "DRAWING SIM"
        int y = _boardStart + 1;
        int x = _sideBoardX + 1;
        auto fontSize = 48;
        drawRect(x, y, _maxSimpleChoiceLength, fontSize * _simpleChoices.size(), SDL_Color{255, 255, 255, 0}, true);
        int ci = -1;
        y -= fontSize;
        for (const auto& tex : _simpleChoiceTextures) {
            y += fontSize;
            ++ci;
            drawTexture(tex, x, y);
            if (_mouseLock) continue;
            int mx, my;
            auto s = SDL_GetMouseState(&mx, &my);
            int w = _maxSimpleChoiceLength;
            int h = fontSize;
            if (!(mx >= x && my >= y && mx <= x + w && my <= h+ y)) continue;
            auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 0};
            this->drawRect(x, y, w, h, color, false);
            this->drawRect(x+1, y+1, w-2, h-2, color, false);
            if (!(s&1)) continue;
            message<PollType> reply;
            reply << _simpleChoices[ci];
            _c->Send(reply);
            SDL_DestroyTexture(_lastTextTex);
            _lastTextTex = nullptr;
            _waitingResponse = false;
            _mouseLock = true;
        }
    }

    void chooseCardsCalc(nlohmann::json j) {
        SDL_DestroyTexture(this->_lastTextTex);
        this->_lastTextTex = _assets->getMessage(j["prompt"]["text"], SDL_Color{255, 0, 0, 0}, _boardStart);
        auto targetID = j["prompt"]["targetID"];
        this->_cardAmount = j["prompt"]["amount"];
        for (const auto& board : _state.boards) {
            if (board.id != targetID) continue;
            for (const auto& card : board.hand)
                _allowedCards.push_back(card.id);
            break;
        }
    }

    void calcAllowedCards(nlohmann::json j) {
        if (_lastRequestType == PollType::Update) return;
        _allowedCards.clear();
        _allowedPlayers.clear();
        _allowedStackMembers.clear();
        _chosenCardIDs.clear();
        _simpleChoices.clear();
        clearSimpleChoiceTextures();
        _waitingResponse = true;

        switch (_lastRequestType) {
        case PollType::GetAction:
            this->getActionCalc();
            break;
        case PollType::Prompt:
            this->promptCalc(j);
            break;
        case PollType::SimplePrompt:
            this->simplePromptCalc(j);
            break;
        case PollType::ChooseCards:
            this->chooseCardsCalc(j);
            break;
        default:
            break;
        }
    }

    void start() {
        this->setup();
        this->_running = true;
        bool clicked = false;
        while (this->_running) {
            if (!_c->Incoming().empty()) {
                auto f = _c->Incoming().pop_front();
                auto msg = f.msg;
                string jstate;
                msg >> jstate;
                auto j = nlohmann::json::parse(jstate);
                _state = MatchState(j);
                _hasState = true;
                _lastRequestType = msg.header.id;
                this->calcAllowedCards(j);
                // request = true;
            }
            // handle events
            while (SDL_PollEvent(this->_event)) {
                switch (_event->type) {
                case SDL_QUIT:
                    this->_running = false;
                    break;
                case SDL_KEYDOWN:
                    this->handleKey(_event->key.keysym.sym);
                    break;
                case SDL_MOUSEBUTTONUP:
                    _mouseLock = false;
                    break;
                }
            }
            this->clear();
            if (_hasState)
                this->draw();
            this->flush();
        }
    }

    void setup() {
        _c = new Client();
        _c->Connect(_host, 9090);

    }

    void handleKey(int key) {
        switch (key) {
        case SDLK_ESCAPE:
            this->_running = false;
            return;
        case 'f':
            this->toggleFullscreen();
            return;
        case SDLK_SPACE:
            if (!_waitingResponse) return;
            message<PollType> reply;
            reply << ACTION_PASS;
            _c->Send(reply);
            _waitingResponse = false;
        }
    }

    void toggleFullscreen() {
        this->_fullscreen = !this->_fullscreen;
        SDL_SetWindowFullscreen(_win, this->_fullscreen);
        SDL_GetWindowSize(this->_win, &this->_wWidth, &this->_wHeight);
    }

    void clear() {
        SDL_RenderClear(_ren);
    }

    void sendAction(PlayerBoardState& state) {
        message<PollType> reply;
        reply << std::to_string(state.id);
        _c->Send(reply);
        SDL_DestroyTexture(_lastTextTex);
        _lastTextTex = nullptr;
    }

    void onClick(PlayerBoardState state) {
        if (!_waitingResponse) return;
        switch (_lastRequestType) {
        case PollType::Prompt:
            sendAction(state);
            break;
        default:
            throw std::runtime_error("attempted to send player while no prompt was given");
            break;
        }
        _waitingResponse = false;
    }

    void sendAction(int i) {
        message<PollType> reply;
        reply << std::to_string(i);
        _c->Send(reply);
        SDL_DestroyTexture(_lastTextTex);
        _lastTextTex = nullptr;
    }

    void onClick(int i) {
        if (!_waitingResponse) return;
        switch (_lastRequestType) {
        case PollType::Prompt:
            sendAction(i);
            break;
        default:
            throw std::runtime_error("attempted to send stack index while no prompt was given");
            break;
        }
        _waitingResponse = false;
    }

    void sendAction(CardState& card) {
        message<PollType> reply;
        switch (card.zone) {
        case Zones::Unknown:
            throw std::runtime_error("Card " + card.cardName + " [" + std::to_string(card.id) + "] has unknown zone");
            return;
        case Zones::Hand:
            //  TODO check that the card is owned by active player
            reply << "play_loot " + std::to_string(card.id);
            break;
        case Zones::Board:
            //  TODO check if the card can be activated
            //  TODO prompt player to choose the activated ability of the card
            reply << "activate " + std::to_string(card.id) + " 0";
            break;
        case Zones::CCard:
            reply << ACTION_ACTIVATE_CHARACTER_CARD;
            break;
        case Zones::Shop:
            //  TODO choose the treasure
            reply << string("buy_treasure 0");
            break;
        case Zones::Stack:
            //  TODO
            break;
        case Zones::LootDiscard:
            //  TODO show the player the discard pile
            break;
        case Zones::TreasureDiscard:
            //  TODO show the player the discard pile
            break;
        case Zones::MonsterDiscard:
            //  TODO show the player the discard pile
            break;
        case Zones::ActiveMonsters:
            //  TODO attack the clicked monster
            break;
        }
        _c->Send(reply);
    }
    
    void answerPrompt(CardState& card) {
        message<PollType> reply;
        reply << std::to_string(card.id);
        _c->Send(reply);
        SDL_DestroyTexture(_lastTextTex);
        _lastTextTex = nullptr;
    }

    void chooseCard(CardState& card) {
        for (const auto& id : _chosenCardIDs)
            if (id == card.id) return;
        _chosenCardIDs.push_back(card.id);
        if (_chosenCardIDs.size() != _cardAmount) return;
        message<PollType> reply;
        string message = std::to_string(_chosenCardIDs[0]);
        for (int i = 1; i < _chosenCardIDs.size(); i++)
            message += " " + std::to_string(_chosenCardIDs[i]);
        reply << message;
        _c->Send(reply);
        SDL_DestroyTexture(_lastTextTex);
        _lastTextTex = nullptr;
        _waitingResponse = false;
    }

    void onClick(CardState card) {
        if (!_waitingResponse) return;
        // std::cout << card.cardName << "\t" << card.id << std::endl;
        switch (_lastRequestType) {
        case PollType::GetAction:
            this->sendAction(card);
            break;
        case PollType::Prompt:
            this->answerPrompt(card);
            break;
        case PollType::ChooseCards:
            this->chooseCard(card);
            return;
        default:
            break;
        }
        _waitingResponse = false;
    }

    // void drawStackMember(StackMemberState& member, int x, int y) {
    //     //  TODO
    // }

    void drawCard(CardState& card, int angle, int x, int y) {
        this->drawTexture(this->_assets->getCard(card.cardName), x, y, angle);
        auto tex = _assets->getMessage("[" + std::to_string(card.id) + "]", SDL_Color{ 255, 0, 255, 0 }, 24);
        drawTexture(tex, x + 2, y + 2);
        SDL_DestroyTexture(tex);
        if (card.counters) {
            tex = _assets->getMessage(std::to_string(card.counters), SDL_Color{ 0, 0, 255, 0 }, 24);
            drawTexture(tex, x + 2, y + 2 + 24);
            SDL_DestroyTexture(tex);
        }
        if (_mouseLock) return;
        bool allowed = false;
        for (const auto& id : _allowedCards)
            if (id == card.id) allowed = true;
        if (!allowed) return;
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        int w = (angle == 0) ? _cardSize.first : _cardSize.second;
        int h = (angle == 0) ? _cardSize.second : _cardSize.first;
        if (mx >= x && my >= y && mx <= x + w && my <= h+ y) {
            auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 0};
            this->drawRect(x, y, w, h, color, false);
            this->drawRect(x+1, y+1, w-2, h-2, color, false);
            if (s&1) {
                onClick(card);
                _mouseLock = true;         
            }
        }
    }

    void drawCardBack(string cardType, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCardBack(cardType), x, y, active ? 0 : 90);
    }

    void draw() {
        // return;
        // draw player separators
        this->drawLine(
            this->_sideBoardX + this->_boardWidth / 2,
            this->_boardStart,
            this->_sideBoardX + this->_boardWidth / 2,
            this->_boardHeight - 1,
            SDL_Color{ 255, 0, 0, 0 });
        this->drawLine(
            this->_sideBoardX,
            this->_boardStart + this->_boardHeight / 2,
            this->_sideBoardX + this->_boardWidth - 1,
            this->_boardStart + this->_boardHeight / 2,
            SDL_Color{ 255, 0, 0, 0 });

        // draw boards
        this->drawBoards();
        // draw decks
        this->drawSideBoard();
        // draw stack
        this->drawStack();

        // draw top text
        if (_lastTextTex) {
            drawTexture(_lastTextTex, _sideBoardX, 0);
        }
        // draw simple choice choices
        this->drawSimplePromptBox();
    }

    void drawSideBoard() {
        this->drawLoot();
        this->drawTreasure();
        this->drawMonsters();
    }

    void drawMonsters() {
        // draw monster deck
        auto count = _state.monsterDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_MONSTER, true, _monsterDeckX, _monsterDeckY);
        // _lastLootDeckCount
        if (count != _lastMonsterDeckCount) {
            _lastMonsterDeckCount = count;
            SDL_DestroyTexture(_lastMonsterDeckCountTex);
            _lastMonsterDeckCountTex = this->_assets->getMessage(std::to_string(_lastMonsterDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastMonsterDeckCountTex, _monsterDeckX + 10, _monsterDeckY + 10);
        // draw monster discard
        count = _state.monsterDiscard.size();
        if (count) {
            auto card = *(_state.monsterDiscard.end() - 1);
            this->drawCard(card, 0, _monsterDiscardX, _monsterDeckY);
        }
        if (count != _lastMonsterDeckCount) {
            _lastMonsterDiscardCount = count;
            SDL_DestroyTexture(_lastMonsterDiscardCountTex);
            _lastMonsterDiscardCountTex = this->_assets->getMessage(std::to_string(_lastMonsterDiscardCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastMonsterDiscardCountTex, _monsterDiscardX + 10, _monsterDeckY + 10);
        // draw monster slots
        auto y = _monsterDeckY + _cardSize.second;
        for (auto& card : _state.monsters) {
            this->drawCard(card, -90, _monsterDiscardX + 20, y);
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            // auto tex = this->_assets->getCard(name);
            // this->drawTexture(tex, _monsterDiscardX + 20, y, -90);
            y += _cardSize.first + 3;
        }
    }

    void drawTreasure() {
        // draw treasure deck
        auto count = _state.treasureDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_TREASURE, true, _treasureDeckX, _treasureDeckY);
        // _lastLootDeckCount
        if (count != _lastTreasureDeckCount) {
            _lastTreasureDeckCount = count;
            SDL_DestroyTexture(_lastTreasureDeckCountTex);
            _lastTreasureDeckCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastTreasureDeckCountTex, _treasureDeckX + 10, _treasureDeckY + 10);
        // draw treasure discard
        count = _state.treasureDiscard.size();
        // std::cout << "\t" << count << std::endl;
        if (count) {
            auto card = *(_state.treasureDiscard.end() - 1);
            this->drawCard(card, 0, _treasureDiscardX, _treasureDeckY);
        }
        if (count != _lastTreasureDiscardCount) {
            _lastTreasureDiscardCount = count;
            SDL_DestroyTexture(_lastTreasureDiscardCountTex);
            _lastTreasureDiscardCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDiscardCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastTreasureDiscardCountTex, 0, _treasureDiscardX + 10, _treasureDeckY + 10);
        // draw shop
        auto y = _treasureDeckY - _cardSize.second;
        for (auto& card : _state.shop) {
            this->drawCard(card, -90, _treasureDiscardX + 20, y);

            y -= _cardSize.first + 3;
        }
    }

    void drawLoot() {
        // draw loot deck
        auto count = _state.lootDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_LOOT, true, _lootDeckX, _lootDeckY);
        if (count != _lastLootDeckCount) {
            _lastLootDeckCount = count;
            SDL_DestroyTexture(_lastLootDeckCountTex);
            _lastLootDeckCountTex = this->_assets->getMessage(std::to_string(_lastLootDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastLootDeckCountTex, _lootDeckX + 10, _lootDeckY + 10);
        // draw loot discard
        count = _state.lootDiscard.size();
        if (count) {
            this->drawCard(*(_state.lootDiscard.end() - 1), 0, _lootDiscardX, _lootDeckY);
        }
        if (count != _lastLootDiscardCount) {
            _lastLootDiscardCount = count;
            SDL_DestroyTexture(_lastLootDiscardCountTex);
            _lastLootDiscardCountTex = this->_assets->getMessage(std::to_string(_lastLootDiscardCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastLootDiscardCountTex, _lootDiscardX + 10, _lootDeckY + 10);
    }

    void drawBoards() {
        for (int i = 0; i < _state.boards.size(); i++) {
            auto space = _state.boards[i];
            int pX = _playerSpaces[i][0];
            int pY = _playerSpaces[i][1];
            int x = pX;
            int y = pY;
            int lootX = pX + 200;
            int looyY = pY + 10;
            if (i == _state.currentI)
                this->drawRect(pX, pY, _boardWidth / 2, _boardHeight / 2, SDL_Color{ 0, 150, 0, 0 }, true);
            if (i == _state.priorityI) {
                int width = _boardWidth / 2;
                int height = _boardHeight / 2;
                this->drawRect(pX, pY, width, height, SDL_Color{ 0, 255, 255, 0 }, false);
                this->drawRect(pX + 1, pY + 1, width - 2, height - 2, SDL_Color{ 0, 255, 255, 0 }, false);
            }

            pX += _cardSize.second - _cardSize.first;
            auto cCard = space.playerCard;

            this->drawCard(cCard, (cCard.active ? 0 : 90), pX + 10, pY + 10);
            pY += 150;
            int betweenCards = 2;
            for (auto& card : space.board) {
                // this->drawCard(card, (card.active ? 0 : 90), pX + 10, pY);
                this->drawCard(card, (card.active ? 0 : 90), pX + 10, pY);
                pX += _cardSize.second + betweenCards;
            }
            for (auto& card : space.hand) {
                this->drawCard(card, 0, lootX, looyY);
                lootX += _cardSize.first + betweenCards;
            }

            pX = _playerSpaces[i][0];
            pY = _playerSpaces[i][1];
            // draw coins
            auto tex = this->_assets->getMessage(std::to_string(space.coinCount), SDL_Color{ 255, 255, 51, 0 }, 48);
            drawTexture(tex, pX + 10, pY + 10);
            SDL_DestroyTexture(tex);
            // draw health
            // tex = this->_assets->getMessage(std::to_string(space.health) + "/" + std::to_string(space.maxHealth), SDL_Color{255, 0, 0, 0}, 48);
            tex = this->_assets->getMessage(std::to_string(space.health), SDL_Color{ 255, 0, 0, 0 }, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 1);
            auto width = getSize(tex).first;
            SDL_DestroyTexture(tex);
            // draw blue health
            if (space.blueHealth) {
                tex = this->_assets->getMessage("+" + std::to_string(space.blueHealth), SDL_Color{ 0, 0, 255, 0 }, 48);
                drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
                width += getSize(tex).first;
                SDL_DestroyTexture(tex);
            }
            // draw max health
            tex = this->_assets->getMessage("/" + std::to_string(space.maxHealth), SDL_Color{ 255, 0, 0, 0 }, 48);
            drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
            SDL_DestroyTexture(tex);
            // draw attack
            tex = this->_assets->getMessage(std::to_string(space.attack), SDL_Color{ 169, 169, 169, 0 }, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 2);
            SDL_DestroyTexture(tex);
            // draw souls
            tex = this->_assets->getMessage(std::to_string(space.soulCount), SDL_Color{ 255, 255, 255, 0 }, 48);
            drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 3);
            SDL_DestroyTexture(tex);

            // draw name
            SDL_Texture* nameTex = _assets->getMessage(space.name, SDL_Color{0, 255, 255, 0}, 24);
            auto size = getSize(nameTex);
            drawRect(x, y, size.first, size.second, SDL_Color{200, 200, 200, 0}, true);
            int w = size.first;
            int h = size.second;
            drawTexture(nameTex, x, y);
            SDL_DestroyTexture(nameTex);
            bool flag = false;
            for (const auto& id : _allowedPlayers)
                if (id == space.id)
                    flag = true;
            if (!flag) continue;
            int mx, my;
            auto s = SDL_GetMouseState(&mx, &my);
            if (mx >= x && my >= y && mx <= x + w && my <= h+ y) {
                auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 200};
                this->drawRect(x, y, w, h, color, false);
                this->drawRect(x+1, y+1, w-2, h-2, color, false);
                if (s&1) {
                    onClick(space);
                    _mouseLock = true;                
                }
            }
        }
    }

    void drawStack() {
        this->drawRect(_stackX, 0, _wWidth - _stackX, _wHeight, SDL_Color{ 169, 169, 169, 0 }, true);
        int yOffset = 10;
        int x = _stackX + 10;
        int y = yOffset;
        int stackI = -1;
        for (auto& si : _state.stack) {
            ++stackI;
            if (si.isCard) {
                this->drawCard(si.card, 0, x, y);
            }
            else {
                // draw the texture
                drawRect(x, y, _cardSize.first, _cardSize.second, SDL_Color{ 0, 0, 255, 0 }, true);
                auto lines = str::split(si.message, "\n");
                for (int i = 0; i < lines.size(); i++) {
                    auto tex = _assets->getMessage(lines[i], SDL_Color{ 200, 200, 100, 0 }, 24);
                    drawTexture(tex, x + 4, y + 4 + 24 * i);
                    SDL_DestroyTexture(tex);
                }
                // if (si.type != ROLL_TYPE) continue;
                bool flag = false;
                for (const auto& i : _allowedStackMembers) {
                    if (i == stackI)
                        flag = true;
                }
                if (flag) {
                    int mx, my;
                    auto s = SDL_GetMouseState(&mx, &my);
                    int w = _cardSize.first;
                    int h = _cardSize.second;
                    if (mx >= x && my >= y && mx <= x + w && my <= h+ y) {
                        auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 200};
                        this->drawRect(x, y, w, h, color, false);
                        this->drawRect(x+1, y+1, w-2, h-2, color, false);
                        if (s&1) {
                            onClick(stackI);    
                            _mouseLock = true;            
                        }
                    }
                }
            }
            y += _cardSize.second + yOffset;
        }
    }

    void flush() {
        SDL_RenderPresent(_ren);
    }

    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
        SDL_SetRenderDrawColor(_ren, color.r, color.g, color.b, 0);
        SDL_RenderDrawLine(_ren, x1, y1, x2, y2);
        SDL_SetRenderDrawColor(_ren, 0, 0, 0, 0);
    }

    void drawTexture(SDL_Texture* texture, int x, int y, int angle = 0) {
        auto size = getSize(texture);
        SDL_Rect dstrect;
        dstrect.w = size.first;
        dstrect.h = size.second;
        dstrect.x = x;
        dstrect.y = y;
        SDL_Point center;
        center.x = 0;
        center.y = 0;
        if (angle == 90) dstrect.x += size.second;
        if (angle == -90) dstrect.y += size.first;
        SDL_RenderCopyEx(_ren, texture, nullptr, &dstrect, angle, &center, SDL_FLIP_NONE);
    }

    void drawRect(int x, int y, int width, int height, SDL_Color color, bool fill) {
        SDL_SetRenderDrawColor(_ren, color.r, color.g, color.b, 0); // last color.a?
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = width;
        r.h = height;
        if (fill)
            SDL_RenderFillRect(this->_ren, &r);
        else SDL_RenderDrawRect(this->_ren, &r);
        SDL_SetRenderDrawColor(this->_ren, 0, 0, 0, 0);
    }
};

int main() {
    string host = "localhost";

    std::cout << "Enter host (" << host << "): ";
    string s1;
    std::getline(std::cin, s1);
    if (s1.size() != 0) host = s1;

    auto wrapper = new ClientWrapper(host, "four-souls-client", "game", false);
    wrapper->start();
    delete wrapper;
    return 0;
}

int ma1in(int argc, char* argv[]) {
    srand(time(0));
    // srand(1);
    BOTS = atoi(argv[1]);
    CONNECTED_PLAYERS = atoi(argv[2]);
    try {
    auto wrapper = new GameWrapper("four-souls", "game", "players.json", false);
    wrapper->start();
    delete wrapper;

    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;

}