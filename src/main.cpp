#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>  
#include <SDL.h>
#include <SDL_ttf.h>

#include "util.hpp"

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

static const vector<string> P1_ACTIONS = vector<string>{
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PLAY_LOOT + " 0",
};

static const vector<string> P2_ACTIONS = vector<string>{
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PLAY_LOOT + " 0"
};

static const vector<string> P3_ACTIONS = vector<string>{
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PASS,
    ACTION_PLAY_LOOT + " 0"
};

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
        std::vector<int> fontSizes = {24};
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

class GameWrapper {
private:
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

    std::thread _gameMatchThread;

    int _playerSpaces[4][2];
    std::pair<int, int> _cardSize;

    int _lastLootDeckCount = 0;
    SDL_Texture* _lastLootDeckCountTex = nullptr;
    int _lastLootDiscardCount = 0;
    SDL_Texture* _lastLootDiscardCountTex = nullptr;
public:
    GameWrapper(string title, string path, bool fullscreen) :
        _title(title),
        _fullscreen(fullscreen)
    {
        this->_game = new Game("game");
        this->_match = _game->createMatch();
        auto p1 = _match->addPlayer("oichii1", _match->getRandomAvailableCharacter(), str::join(P1_ACTIONS.begin(), P1_ACTIONS.end(), "\n"));
        auto p2 = _match->addPlayer("oichii2", _match->getRandomAvailableCharacter(), str::join(P2_ACTIONS.begin(), P2_ACTIONS.end(), "\n"));
        auto p3 = _match->addPlayer("oichii3", _match->getRandomAvailableCharacter(), str::join(P3_ACTIONS.begin(), P3_ACTIONS.end(), "\n"));

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

        const int deckCount = 3;
        int between = 3;
        int startY = (this->_wHeight - deckCount * _cardSize.second) / 2;

        int discardX = between;
        int mainX = discardX + _cardSize.first + between;

        this->_lootDeckX = mainX;
        this->_lootDiscardX = discardX;
        this->_lootDeckY = startY;
    }

    ~GameWrapper() {
        delete _game;
        delete _match;
        delete _assets;

        SDL_DestroyTexture(_lastLootDeckCountTex);
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

    void drawCard(string cardName, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCard(cardName), x, y, active ? 0 : 90);
    }

    void drawCardBack(string cardType, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCardBack(cardType), x, y, active ? 0 : 90);
    }

    void draw() {
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
        //  TODO
        // draw monster deck
        // draw monster discard
        // draw monster slots
    }

    void drawTreasure(const MatchState& state) {
        //  TODO
        // draw treasure deck
        // draw treasure discard
        // draw shop
    }

    void drawLoot(const MatchState& state) {
        // draw loot deck
        this->drawCardBack(CARD_TYPE_LOOT, true, _lootDeckX, _lootDeckY);
        auto count = state.lootDeckCount;
        if (count != _lastLootDeckCount) {
            _lastLootDeckCount = count;
            SDL_DestroyTexture(_lastLootDeckCountTex);
            _lastLootDeckCountTex = this->_assets->getMessage(std::to_string(_lastLootDeckCount), SDL_Color{255, 255, 255, 0}, 24);
        }
        this->drawTexture(_lastLootDeckCountTex, _lootDeckX + 10, _lootDeckY + 10);
        // draw loot discard
        count = state.lootDiscard.size();
        if (count) {
            this->drawCard(*(state.lootDiscard.end()-1), true, _lootDiscardX, _lootDeckY);
        }
        if (count != _lastLootDeckCount) {
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
            this->drawCard(cCard.first, cCard.second, pX+10, pY+10);
            pY += 150;
            int betweenCards = 2;
            for (const auto& card : space.board) {
                this->drawCard(card.first, card.second, pX+10, pY);
                pX += _cardSize.second + betweenCards;
            }
            for (const auto& card : space.hand) {
                this->drawCard(card, true, lootX, looyY);
                lootX += _cardSize.first + betweenCards;
            }
        }
    }

    void drawStack(const MatchState& state) {
        this->drawRect(_stackX, 0, _wWidth - _stackX, _wHeight, SDL_Color{169, 169, 169, 0}, true);
        int yOffset = 10;
        int x = _stackX + 10;
        int y = yOffset;
        for (const auto& si : state.stack) {
            if (si.isCard)
                this->drawCard(si.cardName, true, x, y);
            else
                drawRect(x, y, _cardSize.first, _cardSize.second, SDL_Color{0, 0, 255, 0}, true);
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
        // if (!angle) {
        //     SDL_Rect pos;
        //     pos.x = x;
        //     pos.y = y;
        //     SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
        //     SDL_RenderCopy(_ren, texture, NULL, &pos);
        //     return;
        // }
        auto size = getSize(texture);
        // SDL_Rect srcrect;
        // srcrect.w = size.first;
        // srcrect.h = size.second;
        SDL_Rect dstrect;
        dstrect.w = size.first;
        dstrect.h = size.second;
        dstrect.x = x;
        dstrect.y = y;
        // std::cout << "\t" << "(" << srcrect.x << " ; " << srcrect.y << ")" << "  (" << dstrect.x << " ; " << dstrect.y << ")" << std::endl;
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

int main() {
    // srand(time(0)) =
    srand(1);
    auto wrapper = new GameWrapper("four-souls", "game", false);
    wrapper->start();
    delete wrapper;
    

    // lua_State *L = luaL_newstate();
    // luaL_openlibs(L);
    // lua_register(L, "cfunc", luafunc_cfunc);

    // int r = luaL_dofile(L, "testscript.lua");
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }

    // r = luaL_dofile(L, "test.lua");
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }
    
    // lua_getglobal(L, "funny");
    // if (!lua_isfunction(L, -1)) {
    //     lua_err(L);
    //     return -1;
    // }
    // lua_pushnumber(L, 4.5f);
    // lua_pushnumber(L, 23.f);
    // r = lua_pcall(L, 2, 0, 0);
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }
    
    // lua_getglobal(L, "a");
    // if (!lua_isnumber(L, -1)) {
    //     lua_err(L);
    //     return -1;
    // }
    // float result = (float)lua_tonumber(L, -1);
    // cout << "a = " << result << endl;
    // lua_close(L);
}