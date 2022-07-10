#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>  
#include <SDL.h>

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

// int luafunc_cfunc(lua_State *L) {
//     auto f = (float)lua_tonumber(L, -1);
//     lua_pushnumber(L, f * 10);
//     return 1; // the amount of return values
// }

static const vector<string> P1_ACTIONS = vector<string>{
    PASS_RESPONSE
};
static const vector<string> P2_ACTIONS = vector<string>{
    PASS_RESPONSE
};
static const vector<string> P3_ACTIONS = vector<string>{
    PASS_RESPONSE
};

std::pair<int, int> getSize(SDL_Texture *texture) {
    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return std::make_pair(size.x, size.y);
}

class AssetsManager {
private:
    SDL_Renderer* _ren = nullptr;

    std::map<string, SDL_Texture*> _textureMap;

public:
    AssetsManager(SDL_Renderer* ren) :
        _ren(ren) 
    {

    }

    ~AssetsManager() {
        for (const auto& [key, value] : _textureMap) SDL_DestroyTexture(value);
    }

    std::pair<int, int> cardSize() { return getSize(_textureMap.begin()->second); }

    void addCard(Card* card) {
        auto tex = this->loadBMP(card->imagePath());
        this->_textureMap[card->name()] = tex;
    }

    SDL_Texture* getCard(string cardName) {
        if (!_textureMap.count(cardName)) throw std::runtime_error("no image for card " + cardName);
        return _textureMap[cardName];
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
    int _wWidth;
    int _wHeight;
    int _stackX;
    int _boardWidth;
    int _boardHeight;

    std::thread _gameMatchThread;

    int _playerSpaces[4][2];
    std::pair<int, int> _cardSize;
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
        auto stackWidth = 150;
        this->_stackX = this->_wWidth - stackWidth;

        this->_boardWidth = this->_stackX - 1;
        this->_boardHeight = this->_wHeight;

        this->_playerSpaces[0][0] = 0;
        this->_playerSpaces[0][1] = 0;

        this->_playerSpaces[1][0] = _boardWidth / 2+1;
        this->_playerSpaces[1][1] = 0;

        this->_playerSpaces[2][0] = _boardWidth / 2+1;
        this->_playerSpaces[2][1] = _boardHeight / 2+1;

        this->_playerSpaces[3][0] = 0;
        this->_playerSpaces[3][1] = _boardHeight / 2+1;

        this->_assets = new AssetsManager(this->_ren);
        auto allCards = this->_game->getAllCards();
        for (const auto& c : allCards)
            this->_assets->addCard(c);
        this->_cardSize = this->_assets->cardSize();
    }

    ~GameWrapper() {
        delete _game;
        delete _match;
        delete _assets;
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
        //  TODO add rotation
        this->drawTexture(this->_assets->getCard(cardName), x, y);
    }

    void draw() {
        // draw player separators
        this->drawLine(this->_boardWidth / 2, 0, this->_boardWidth / 2, this->_boardHeight-1, SDL_Color{255, 0, 0, 0});
        this->drawLine(0, this->_boardHeight / 2, this->_boardWidth-1, this->_boardHeight / 2, SDL_Color{255, 0, 0, 0});

        // get game state
        auto state = this->_match->getState();
        // draw boards
        this->drawBoards(state);
        // draw stack
        this->drawStack(state);
    }

    void drawBoards(const MatchState& state) {
        for (int i = 0; i < state.boards.size(); i++) {
            auto space = state.boards[i];
            int pX = _playerSpaces[i][0];
            int pY = _playerSpaces[i][1];
            if (i == state.currentI)
                this->drawRect(pX, pY, _boardWidth/2, _boardHeight/2, SDL_Color{0, 150, 0, 0}, true);
            if (i == state.priorityI) {
                int width = _boardWidth/2;
                int height = _boardHeight/2;
                this->drawRect(pX, pY, width, height, SDL_Color{0, 255, 255, 0}, false);
                this->drawRect(pX+1, pY+1, width-2, height-2, SDL_Color{0, 255, 255, 0}, false);
            }
            auto cCard = space.playerCard;
            this->drawCard(cCard.first, cCard.second, pX+10, pY+10);
            pY += 150;
            for (const auto& card : space.board) {
                this->drawCard(card.first, card.second, pX+10, pY);
                pX += _cardSize.second + 2;
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

    void drawTexture(SDL_Texture *texture, int x, int y) {
        SDL_Rect pos;
        pos.x = x;
        pos.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
        SDL_RenderCopy(_ren, texture, NULL, &pos);
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
    // srand(time(0));
    srand(0);
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