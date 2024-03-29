#include "game/core.hpp"
#include "game/cards.hpp"
#include "game/match.hpp"
#include "game/types.hpp"



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
        auto allCards = _game->getAllCards();
        int pcount = 0;
        for (auto& conn : _server->GetClients()) {
            pcount++;
            auto p = new ConnectedPlayer(
                "player " + std::to_string(pcount),
                _match->getRandomAvailableCharacter(),
                _match->newCardID(),
                _allCards,
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
                _match->newCardID(),
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
        this->drawTexture(this->_assets->getCard(card.name), x, y, card.active ? 0 : 90);
        auto tex = _assets->getMessage("["+std::to_string(card.id)+"]", SDL_Color{255, 0, 255, 0}, 24);
        drawTexture(tex, x + 2, y + 2);
        SDL_DestroyTexture(tex);
        if (card.counters) {
            tex = _assets->getMessage(std::to_string(card.counters), SDL_Color{0, 0, 255, 0}, 24);
            drawTexture(tex, x + 2, y + 2 + 24);
            SDL_DestroyTexture(tex);
        }
    }

    void drawCard(string cardKey, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCard(cardKey), x, y, active ? 0 : 90);
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
        for (int i = 0; i < state.monsters.size(); i++) {

            auto card = state.monsters[i];
            this->drawTexture(this->_assets->getCard(card.name),_monsterDiscardX + 20, y, -90);

            auto data = state.monsterDataArr[i];
            auto health = data.health;
            auto tex = _assets->getMessage(std::to_string(health), SDL_Color{255, 0, 0, 0}, 48);
            drawTexture(tex, _monsterDiscardX + 30, y + 30);
            if (data.blueHealth) {
                auto btex = _assets->getMessage("+" + std::to_string(data.blueHealth), SDL_Color{255, 0, 0, 0}, 48);
                drawTexture(btex, _monsterDiscardX + 30 + getSize(tex).first, y + 30);
                SDL_DestroyTexture(btex);
            }
            SDL_DestroyTexture(tex);
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
            this->drawTexture(this->_assets->getCard(card.name),_treasureDiscardX + 20, y, -90);
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

/*
recording file format:
name: game<time/seed>.fsr
encoded in json
example:
{
    "seed": <game seed>,
    "players": {
        "player1": [
            "$PASS",
            "play_loot 0"
        ],
        "player2": [
            "$PASS",
            "$PASS",
            "$PASS"
        ]
    }
}
*/

int main(int argc, char* argv[]) {
    // auto seed = time(0);
    auto seed = 1658171179;
    std::cout << "SEED: " << seed << std::endl;
    srand(seed);
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