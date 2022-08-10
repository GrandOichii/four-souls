#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "sound.hpp"
#include "common.hpp"

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

enum CardSize : int {
    SMALL,
    LARGE
};

static int MAX_WIDTH = 40;

class AssetsManager {
private:
    SDL_Renderer* _ren = nullptr;

    std::map<string, std::pair<SDL_Texture*, SDL_Texture*>> _textureMap;
    std::map<string, SDL_Texture*> _cardBacks;

    std::map<int, Font*> _fontMap;
public:
    AssetsManager(SDL_Renderer* ren, string assetsPath) :
        _ren(ren) 
    {
        this->load(assetsPath);
    }

    ~AssetsManager() {
        for (const auto& [key, value] : _textureMap) {
            SDL_DestroyTexture(value.first);
            SDL_DestroyTexture(value.second);
        }
        for (const auto& [key, value] : _cardBacks) SDL_DestroyTexture(value);
        for (const auto& [key, value] : _fontMap) delete value;
    }

    SDL_Texture* createCardBack(string deckName) {
        auto result = SDL_CreateTexture(_ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 87, 121);
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = 87;
        r.h = 121;
        SDL_SetRenderTarget(_ren, result);
        SDL_SetRenderDrawColor(_ren, 169, 169, 169, 0);
        SDL_RenderFillRect(_ren, &r);
        auto top = getMessage(deckName, SDL_Color{0, 255, 0, 0}, 24);
        auto size = getSize(top);
        r.w = size.first;
        r.h = size.second;
        SDL_RenderCopy(_ren, top, NULL, &r);
        SDL_DestroyTexture(top);
        auto bottom = getMessage("deck", SDL_Color{0, 255, 0, 0}, 24);
        size = getSize(bottom);
        r.x = 10;
        r.w = size.first;
        r.h = size.second;
        r.y = 24;
        SDL_RenderCopy(_ren, bottom, NULL, &r);
        SDL_DestroyTexture(bottom);
        SDL_RenderPresent(_ren);
        SDL_SetRenderTarget(_ren, NULL);

        return result;
    }

    void load(string assetsPath) {
        auto config = fs::readJS(fs::join(assetsPath, "config.json"));
        // load font
        string fontPath = fs::join(assetsPath, config["font"]);
        std::vector<int> fontSizes = {24, 48};
        for (auto& fs : fontSizes)
            _fontMap[fs] = new Font(fontPath.c_str(), fs, _ren);
        // card backs images
        vector<string> names = {"loot", "treasure", "monster"};
        for (const auto& name : names) {
            if (config.contains(name + "_back"))
                _cardBacks[name] = loadBMP(fs::join(assetsPath, config[name + "_back"]));
            else _cardBacks[name] = createCardBack(name);
        }
        // add cards
        for (const auto& [key, value] : config["cards"].items()){
            SDL_Texture* small = nullptr;
            SDL_Texture* large = nullptr;
            if (value.contains("small")) {
                try {
                    small = loadBMP(fs::join(assetsPath, value["small"]));
                } catch (std::exception& ex) {
                    std::cout << "FAILED LOADING " << value["large"];
                    throw ex;
                }
            }
            if (value.contains("large")) {
                try {
                    large = loadBMP(fs::join(assetsPath, value["large"]));
                } catch (std::exception& ex) {
                    std::cout << "FAILED LOADING " << value["large"];
                    throw ex;
                }
            }
            _textureMap[key] = std::make_pair(small, large);
        }
    }

    void createCard(string name, string text) {
        if (!_textureMap.count(name)) _textureMap[name] = std::make_pair(nullptr, nullptr);
        auto pair = _textureMap[name];
        if (!pair.first) {
            auto small = SDL_CreateTexture(_ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 87, 121);
            SDL_Rect r;
            r.x = 0;
            r.y = 0;
            r.w = 87;
            r.h = 121;
            SDL_SetRenderTarget(_ren, small);
            SDL_SetRenderDrawColor(_ren, 169, 169, 169, 0);
            SDL_RenderFillRect(_ren, &r);
            auto top = getMessage(name, SDL_Color{0, 255, 0, 0}, 24);
            auto size = getSize(top);
            r.w = size.first;
            r.h = size.second;
            SDL_RenderCopy(_ren, top, NULL, &r);
            SDL_DestroyTexture(top);
            SDL_RenderPresent(_ren);
            _textureMap[name].first = small;
        }
        if (!pair.second) {
            // 437 610
            auto large = SDL_CreateTexture(_ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 437, 610);
            SDL_Rect r;
            r.x = 0;
            r.y = 0;
            r.w = 437;
            r.h = 610;
            SDL_SetRenderTarget(_ren, large);
            SDL_SetRenderDrawColor(_ren, 169, 169, 169, 0);
            SDL_RenderFillRect(_ren, &r);

            auto top = getMessage(name, SDL_Color{0, 255, 0, 0}, 24);
            auto size = getSize(top);
            r.w = size.first;
            r.h = size.second;
            SDL_RenderCopy(_ren, top, NULL, &r);
            SDL_DestroyTexture(top);
            SDL_RenderPresent(_ren);

            if (text.size()) {
                r.y = 40;
                auto lines = str::split(text, "\n");
                for (const auto& line : lines) {
                    auto split = str::widthSplit(line, MAX_WIDTH);
                    for (const auto& s : split) {
                        auto tex = getMessage(s, SDL_Color{0, 255, 0, 0}, 24);
                        size = getSize(tex);
                        r.w = size.first;
                        r.h = size.second;
                        SDL_RenderCopy(_ren, tex, NULL, &r);
                        SDL_DestroyTexture(tex);
                        r.y += 24;
                    }
                }
                SDL_RenderPresent(_ren);
            }

            _textureMap[name].second = large;
        }
        SDL_SetRenderTarget(_ren, NULL);
    }

    std::pair<int, int> cardSize() { return getSize(_textureMap.begin()->second.first); }

    void addCardBack(string cardType, string path) {
        auto tex = this->loadBMP(path);
        this->_cardBacks[cardType] = tex;
    }

    SDL_Texture* getCard(string cardName, CardSize size) {
        if (!_textureMap.count(cardName)) throw std::runtime_error("no image for card " + cardName);
        auto pair = _textureMap[cardName];
        switch(size) {
        case CardSize::SMALL:
            return pair.first;
        case CardSize::LARGE:
            return pair.second;
        }
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
        // std::cout << "RENDERING TEX " << message << std::endl;
        if (!_fontMap.count(fontSize)) throw std::runtime_error("no font with size " + std::to_string(fontSize));
        return _fontMap[fontSize]->get(message, color);
    }
};

struct Rect {
    int x, y;
    int w, h;
    // Rect(int x, int y, int w, int h) : 
    //     x(x),
    //     y(y),
    //     w(w),
    //     h(h) {}
};

class Window {
protected:
    const unsigned char * _keys;
    AssetsManager* _assets = nullptr;

    SDL_Window* _win = nullptr;
    SDL_Renderer* _ren = nullptr;

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
    std::pair<int, int> _cardSize = std::make_pair(87, 121);

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

    bool _fullscreen;
    string _title;

    std::map<int, std::pair<int, int>> _cardLocs;

public:
    Window(string title, string assetsPath, bool fullscreen):
        _title(title),
        _fullscreen(fullscreen)
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        auto width = DM.w;
        auto height = DM.h;
        SDL_StartTextInput();
        this->_win = SDL_CreateWindow(this->_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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
        _keys = SDL_GetKeyboardState(NULL);

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

        this->_assets = new AssetsManager(this->_ren, assetsPath);

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

    ~Window() {
        delete _assets;

        SDL_DestroyTexture(_lastLootDeckCountTex);
        SDL_DestroyTexture(_lastLootDiscardCountTex);
        SDL_DestroyTexture(_lastTreasureDeckCountTex);
        SDL_DestroyTexture(_lastTreasureDiscardCountTex);
        SDL_DestroyTexture(_lastMonsterDeckCountTex);
        SDL_DestroyTexture(_lastMonsterDiscardCountTex);
        SDL_DestroyWindow(_win);
        SDL_Quit();
    }

    AssetsManager* assets() { return _assets; }

    void toggleFullscreen() {
        this->_fullscreen = !this->_fullscreen;
        SDL_SetWindowFullscreen(_win, this->_fullscreen);
        SDL_GetWindowSize(this->_win, &this->_wWidth, &this->_wHeight);
    }

    void draw(MatchState& state) {
        _cardLocs.clear();
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
        this->drawBoards(state);
        // draw decks
        this->drawSideBoard(state);
        // draw stack
        this->drawStack(state);
    }

    void drawSideBoard(MatchState& state) {
        this->drawLoot(state);
        this->drawTreasure(state);
        this->drawMonsters(state);
    }

    void drawMonsters(MatchState& state) {
        // draw monster deck
        auto count = state.monsterDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_MONSTER, true, _monsterDeckX, _monsterDeckY);
        // _lastLootDeckCount
        if (count != _lastMonsterDeckCount) {
            _lastMonsterDeckCount = count;
            SDL_DestroyTexture(_lastMonsterDeckCountTex);
            _lastMonsterDeckCountTex = this->_assets->getMessage(std::to_string(_lastMonsterDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastMonsterDeckCountTex, _monsterDeckX + 10, _monsterDeckY + 10);
        // draw monster discard
        count = state.monsterDiscard.size();
        if (count) {
            auto card = *(state.monsterDiscard.end() - 1);
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
        int monstersX = _monsterDiscardX + 10;
        for (int i = 0; i < state.monsters.size(); i++) {
            auto card = state.monsters[i];
            this->drawCard(card, -90, monstersX, y);
            auto x = monstersX + _assets->cardSize().second + 10;
            auto data = state.monsterDataArr[i];
            // draw health
            auto health = data.health;
            auto tex = _assets->getMessage(std::to_string(health), SDL_Color{255, 0, 0, 0}, 24);
            this->drawTexture(tex, x, y);
            if (data.blueHealth) {
                auto btex = _assets->getMessage("+" + std::to_string(data.blueHealth), SDL_Color{255, 0, 0, 0}, 48);
                this->drawTexture(btex, x + getSize(tex).first, y + 30);
                SDL_DestroyTexture(btex);
            }
            SDL_DestroyTexture(tex);
            // draw roll
            tex = _assets->getMessage(std::to_string(data.roll), SDL_Color{255, 255, 255, 0}, 24);
            this->drawTexture(tex, x, y + 24);
            SDL_DestroyTexture(tex);
            // draw power
            tex = _assets->getMessage(std::to_string(data.power), SDL_Color{169, 169, 169, 0}, 24);
            this->drawTexture(tex, x, y + 48);
            SDL_DestroyTexture(tex);
            y += _cardSize.first + 3;
        }
    }

    void drawTreasure(MatchState& state) {
        // draw treasure deck
        this->drawTreasureDeck(state);
        // draw treasure discard
        auto count = state.treasureDiscard.size();
        if (count) {
            auto card = *(state.treasureDiscard.end() - 1);
            this->drawCard(card, 0, _treasureDiscardX, _treasureDeckY);
        }
        if (count != _lastTreasureDiscardCount) {
            _lastTreasureDiscardCount = count;
            SDL_DestroyTexture(_lastTreasureDiscardCountTex);
            _lastTreasureDiscardCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDiscardCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastTreasureDiscardCountTex, _treasureDiscardX + 10, _treasureDeckY + 10);
        // draw shop
        auto y = _treasureDeckY - _cardSize.second;
        for (auto& card : state.shop) {
            this->drawCard(card, -90, _treasureDiscardX + 20, y);

            y -= _cardSize.first + 3;
        }
    }

    virtual void drawTreasureDeck(MatchState& state) {
        auto count = state.treasureDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_TREASURE, true, _treasureDeckX, _treasureDeckY);
        // _lastLootDeckCount
        if (count != _lastTreasureDeckCount) {
            _lastTreasureDeckCount = count;
            SDL_DestroyTexture(_lastTreasureDeckCountTex);
            _lastTreasureDeckCountTex = this->_assets->getMessage(std::to_string(_lastTreasureDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastTreasureDeckCountTex, _treasureDeckX + 10, _treasureDeckY + 10);
    }

    void drawLoot(MatchState& state) {
        // draw loot deck
        auto count = state.lootDeckCount;
        if (count) this->drawCardBack(CARD_TYPE_LOOT, true, _lootDeckX, _lootDeckY);
        if (count != _lastLootDeckCount) {
            _lastLootDeckCount = count;
            SDL_DestroyTexture(_lastLootDeckCountTex);
            _lastLootDeckCountTex = this->_assets->getMessage(std::to_string(_lastLootDeckCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastLootDeckCountTex, _lootDeckX + 10, _lootDeckY + 10);
        // draw loot discard
        count = state.lootDiscard.size();
        if (count) {
            this->drawCard(*(state.lootDiscard.end() - 1), 0, _lootDiscardX, _lootDeckY);
        }
        if (count != _lastLootDiscardCount) {
            _lastLootDiscardCount = count;
            SDL_DestroyTexture(_lastLootDiscardCountTex);
            _lastLootDiscardCountTex = this->_assets->getMessage(std::to_string(_lastLootDiscardCount), SDL_Color{ 255, 255, 255, 0 }, 24);
        }
        this->drawTexture(_lastLootDiscardCountTex, _lootDiscardX + 10, _lootDeckY + 10);
    }

    void drawPlayerBoard(MatchState& state, int playerI) {
        auto pboard = state.boards[playerI];
        int pX = _playerSpaces[playerI][0];
        int pY = _playerSpaces[playerI][1];
        int x = pX;
        int y = pY;
        int lootX = pX + 200;
        int lootY = pY + 10;
        if (playerI == state.currentI)
            this->drawRect(pX, pY, _boardWidth / 2, _boardHeight / 2, SDL_Color{ 0, 150, 0, 0 }, true);
        if (playerI == state.priorityI) {
            int width = _boardWidth / 2;
            int height = _boardHeight / 2;
            this->drawRect(pX, pY, width, height, SDL_Color{ 0, 255, 255, 0 }, false);
            this->drawRect(pX + 1, pY + 1, width - 2, height - 2, SDL_Color{ 0, 255, 255, 0 }, false);
        }

        pX += _cardSize.second - _cardSize.first;
        auto cCard = pboard.playerCard;
        this->drawCard(cCard, (cCard.active ? 0 : 90), pX + 10, pY + 10);
        pY += 150;
        int betweenCards = 2;
        // draw board
        for (auto& card : pboard.board) {
            // this->drawCard(card, (card.active ? 0 : 90), pX + 10, pY);
            this->drawCard(card, (card.active ? 0 : 90), pX + 10, pY);
            pX += _cardSize.second + betweenCards;
        }
        // draw hand
        int xDiff = _cardSize.first + betweenCards;
        int w = xDiff * pboard.hand.size();
        int bWidth = (_boardWidth) / 2 - 200;
        int cardW = _assets->cardSize().first;
        if (w > bWidth) {
            xDiff = (bWidth - cardW) / (pboard.hand.size() - 1);
            if (xDiff < cardW)
                cardW = xDiff;
        }
        for (auto& card : pboard.hand) {
            Rect bBox{
                lootX, lootY,
                (&card == &pboard.hand.back()) ? _assets->cardSize().first : cardW, _assets->cardSize().second
            };
            this->drawCard(card, 0, lootX, lootY, &bBox);
            lootX += xDiff;
        }

        pX = _playerSpaces[playerI][0];
        pY = _playerSpaces[playerI][1];
        // draw coins
        auto tex = this->_assets->getMessage(std::to_string(pboard.coinCount), SDL_Color{ 255, 255, 51, 0 }, 48);
        this->drawTexture(tex, pX + 10, pY + 10);
        SDL_DestroyTexture(tex);
        // draw health
        // tex = this->_assets->getMessage(std::to_string(pboard.health) + "/" + std::to_string(pboard.maxHealth), SDL_Color{255, 0, 0, 0}, 48);
        tex = this->_assets->getMessage(std::to_string(pboard.health), SDL_Color{ 255, 0, 0, 0 }, 48);
        this->drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 1);
        auto width = getSize(tex).first;
        SDL_DestroyTexture(tex);
        // draw blue health
        if (pboard.blueHealth) {
            tex = this->_assets->getMessage("+" + std::to_string(pboard.blueHealth), SDL_Color{ 0, 0, 255, 0 }, 48);
            this->drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
            width += getSize(tex).first;
            SDL_DestroyTexture(tex);
        }
        // draw max health
        tex = this->_assets->getMessage("/" + std::to_string(pboard.maxHealth), SDL_Color{ 255, 0, 0, 0 }, 48);
        this->drawTexture(tex, pX + 10 + width, pY + 10 + (48 + 2) * 1);
        SDL_DestroyTexture(tex);
        // draw attack
        tex = this->_assets->getMessage(std::to_string(pboard.attack), SDL_Color{ 169, 169, 169, 0 }, 48);
        this->drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 2);
        SDL_DestroyTexture(tex);
        // draw souls
        //  TODO
        int sresult = 0;
        for (const auto& card : pboard.souls)
            sresult += card.soulCount;
        tex = this->_assets->getMessage(std::to_string(sresult), SDL_Color{ 255, 255, 255, 0 }, 48);
        this->drawTexture(tex, pX + 10, pY + 10 + (48 + 2) * 3);
        SDL_DestroyTexture(tex);

        // draw name
        this->drawPlayerName(pboard, x, y);
    }

    virtual std::pair<int, int> drawPlayerName(PlayerBoardState& pboard, int x, int y) {
        SDL_Texture* nameTex = _assets->getMessage(pboard.name, SDL_Color{0, 255, 255, 0}, 24);
        auto size = getSize(nameTex);
        this->drawRect(x, y, size.first, size.second, SDL_Color{200, 200, 200, 0}, true);
        int w = size.first;
        int h = size.second;
        this->drawTexture(nameTex, x, y);
        SDL_DestroyTexture(nameTex);
        return size;
    }

    void drawBoards(MatchState& state) {
        for (int i = 0; i < state.boards.size(); i++) {
            drawPlayerBoard(state, i);
        }
    }

    virtual void drawSpecialStackMember(MatchState& state, int stackI, int x, int y) {
        // draw the texture
        auto si = state.stack[stackI];
        this->drawRect(x, y, _cardSize.first, _cardSize.second, SDL_Color{ 0, 0, 255, 0 }, true);
        auto lines = str::split(si.message, "\n");
        for (int i = 0; i < lines.size(); i++) {
            auto tex = _assets->getMessage(lines[i], SDL_Color{ 200, 200, 100, 0 }, 24);
            this->drawTexture(tex, x + 4, y + 4 + 24 * i);
            SDL_DestroyTexture(tex);
        }
    }

    void drawStack(MatchState& state) {
        this->drawRect(_stackX, 0, _wWidth - _stackX, _wHeight, SDL_Color{ 169, 169, 169, 0 }, true);
        int yOffset = 10;
        int x = _stackX + 10;
        int y = yOffset;
        int stackI = -1;
        for (auto& si : state.stack) {
            ++stackI;
            if (si.isCard) {
                this->drawCard(si.card, 0, x, y);
            }
            else {
                this->drawSpecialStackMember(state, stackI, x, y);
            }
            for (const auto& target : si.targets) {
                auto loc = _cardLocs[target];
                drawLine(x, y, loc.first, loc.second, SDL_Color{0, 255, 0, 0});
            }
            y += _cardSize.second + yOffset;
        }
    }

    virtual void drawCard(CardState& card, int angle, int x, int y, Rect * bBox = nullptr) {
        auto cardTex = this->_assets->getCard(card.cardName, CardSize::SMALL);
        _cardLocs[card.id] = std::make_pair(x + 10, y + 10);
        this->drawTexture(cardTex, x, y, angle);
        auto tex = _assets->getMessage("[" + std::to_string(card.id) + "]", SDL_Color{ 255, 0, 255, 0 }, 24);
        this->drawTexture(tex, x + 2, y + 2);
        SDL_DestroyTexture(tex);
        if (card.counters) {
            auto tex = _assets->getMessage(std::to_string(card.counters), SDL_Color{ 0, 0, 255, 0 }, 24);
            this->drawTexture(tex, x + 2, y + 2 + 24);
            SDL_DestroyTexture(tex);
        }
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        auto size = getSize(cardTex);
        int w = size.first;
        int h = size.second;
        if (mx >= x && my >= y && mx <= x + w && my <= h+ y) {
            SDL_PumpEvents();
            if (_keys[SDL_SCANCODE_LSHIFT]) {
                auto tex = this->_assets->getCard(card.cardName, CardSize::LARGE);
                auto size = getSize(tex);
                this->drawTexture(tex, _stackX - size.first, 0);
            }
        }
    }

    void drawCardBack(string cardType, bool active, int x, int y) {
        this->drawTexture(this->_assets->getCardBack(cardType), x, y, active ? 0 : 90);
    }

    void clear() {
        SDL_RenderClear(_ren);
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