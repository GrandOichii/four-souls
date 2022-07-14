#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>  

#define CONNECTED_PLAYERS 0
#define BOTS 2

// #include <asio.hpp>
// #include <asio/ts/buffer.hpp>
// #include <asio/ts/internet.hpp>

#include "network/olc_network.hpp"

#include "util.hpp"


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

enum ResponseType : uint8_t {
    PlayLootCard,
    Pass
};

class Server : public server_interface<int> {
private:
    int _connections = 0;
    int _maxConnections;

public:
    Server(uint16_t port, int maxConnections) :
        server_interface<int>(port),
        _maxConnections(maxConnections)
    {}

    tsqueue<owned_message<int>>& Incoming() {
        return m_qMessagesIn;
    }

    std::deque<std::shared_ptr<connection<int>>>& GetClients() {
        return m_deqConnections;
    }

    void WaitForMessages() {
        this->Update(1, true);
    }

    std::shared_ptr<connection<int>>& LastClient() {
        return _lastClient;
    }

    message<int>& LastMessage() {
        return _lastMsg;
    }

    int Connections() {
        return _connections;
    }

protected:
    std::shared_ptr<connection<int>> _lastClient;
    message<int> _lastMsg;

    virtual bool OnClientConnect(std::shared_ptr<connection<int>> client)
    {
        // std::cout << "Client connected, id: " << client->GetID() << std::endl;
        if (_maxConnections == _connections) return false;
        _connections++;
        std::cout << "NEW PLAYER " << _connections << "\t" << _maxConnections << std::endl;
        return true;
    }

    // Called when a client appears to have disconnected
    virtual void OnClientDisconnect(std::shared_ptr<connection<int>> client)
    {
        std::cout << "Client with id " << client->GetID() << " disconnected" << std::endl;
    }

    // Called when a message arrives
    virtual void OnMessage(std::shared_ptr<connection<int>> client, message<int> &msg)
    {
        // fix bug on too many characters
        _lastClient = client;
        _lastMsg = msg;
        /*
      switch (msg.header.id) {
      case int::ServerPing: {
        std::wcout << "[" << msg.header.name.data() << "]: Ping the server\n";

        // Simply bounce message back to client
        client->send(msg);
        break;
      }

      case int::MessageAll: {
        std::wcout << "[" << msg.header.name.data() << "]: Send the message to all user\n";

        //Construct a new message and send it to all clients
        message<int> __msg;
        __msg.header.id = int::ServerMessage;
        __msg.header.name = msg.header.name;
        message_all_clients(__msg, client);
        break;
      }

      case int::JoinServer: {
        std::wcout << "[" << msg.header.name.data() << "] Join the server\n";
        break;
      }

      case int::PassString: {
        std::wcout << "[" << msg.header.name.data() << "]: " << msg.data.data() << '\n';
        break;
      }
      }
      */
    }
};

class ConnectedPlayer : public Player {
private:
    Server* _server;
    std::shared_ptr<connection<int>> _conn;
public:
    ConnectedPlayer(std::string name, CharacterCard* card, int id, Server* server, std::shared_ptr<connection<int>> conn) :
        Player(name, card, id),
        _server(server),
        _conn(conn)
    {}

    ~ConnectedPlayer() {

    }

    string getResponse() {
        message<int> msg;
        msg.header.id = 0;
        msg << string("Do what?");
        _server->MessageClient(_conn, msg);
        _server->WaitForMessages();
        auto response = _server->LastMessage();
        string result;
        response >> result;
        return result;
    }

    string promptAction(const MatchState& state) { 
        return getResponse();
    }

    string promptResponse(const MatchState& state, string text, string choiceType, vector<int> choices) { 
        auto result = getResponse();
        return (result == "$PASS" ? "$FIRST" : result);
    }

    string promptSimpleResponse(const MatchState& state, string text, vector<string> choices) {
        auto result = getResponse();
        return (result == "$PASS" ? "$FIRST" : result);
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
        // auto j = fs::readJS(playersPath);
        // for (const auto& [key, value] : j.items()) {
        //     // vector<string> actions;
        //     // vector<string> responses;
        //     // for (const auto& [key, value] : pvalue["actions"].items()) actions.push_back(value);
        //     // for (const auto& [key, value] : pvalue["responses"].items()) responses.push_back(value);
        //     // auto aj = str::join(actions.begin(), actions.end(), "\n");
        //     // auto rj = str::join(responses.begin(), responses.end(), "\n");
        //     auto p = _match->addPlayer(
        //         value["name"], 
        //         _match->getRandomAvailableCharacter(), 
        //         fs::readFile(fs::join(path, value["script"]).c_str())
        //     );
        // }

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
        // resolve weird error (card with no name)
        // implement dice rolling (is part of loot card cost, dice result goes BEFORE loot card)
        // example: [dice roll], [wheel of fortune]
        // when resolving, searches for first item on stack with type "dice", uses it's value
        // dice rolls can be rerolled/modified using items and loot cards
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


// int main()
// {
//     srand(0);
//     Server server(9090);

//     while (true) {
//         server.Update(-1, false);
//         if (server.GetClients().size()) {
//             auto client = server.GetClients()[0];
//             message<int> msg;
//             msg.header.id = 0;
//             msg << string("Burger?");
//             server.MessageClient(client, msg);
//             server.WaitForMessages();
//             auto response = server.LastMessage();
//             std::cout << "received response" << std::endl;
//             std::cout << response << std::endl;
//             string re;
//             response >> re;
//             std::cout << "Clients response: " << re << std::endl;
//         }
//     }

//     return 0;
// }

class Client : public client_interface<int> {
public:
    void ping_server()
    {
        // message<int> msg;
        // msg.header.id = int::ServerPing;
        // msg.header.name = user_name;
        // send(msg);
    }

    void message_all()
    {
        // message<int> msg;
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

        // message<int> msg;
        // msg.header.id = int::JoinServer;
        // msg.header.name = user_name;
        // send(msg);
    }

    void send_msg(std::wstring &__data)
    {
        // message<int> msg;
        // msg.header.id = int::PassString;
        // msg.header.name = user_name;
        // for (unsigned int i{}, j{}; j < __data.size(); ++i, ++j)
        //     msg.data[i] = __data[j];

        // send(msg);
    }

public:
    std::array<wchar_t, 256> user_name{};
};

int main1() {
    Client c;
    c.Connect("localhost", 9090);
    if (!c.IsConnected()) {
        std::cerr << "Something went wrong, could not connect\n";
        return 0;
    }
    while (1) {
        int count = 0;
        if (!c.Incoming().empty()) {
            auto msg = c.Incoming().pop_front().msg;
            string m;
            msg >> m;
            std::cout << "Recived: " << m << std::endl;
            std::cout << "Your response: ";
            string response;
            std::getline(std::cin, response);
            message<int> reply;
            if (!response.size())
                response = "$PASS";
            reply << response;
            reply.header.id = 0;
            c.Send(reply);
        }
    }
}

int main() {
    // srand(time(0));
    srand(0);
    auto wrapper = new GameWrapper("four-souls", "game", "players.json", false);
    wrapper->start();
    delete wrapper;
    // Message<CustomMsgTypes> msg;
    // msg.header.id = CustomMsgTypes::Attack;

}