#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include <regex>

#include "common.hpp"
#include "window.hpp"

const string COMMON_CAPTURE = "\\w\\ \\'\\!\\?\\.\\(\\)\\-\\\\";
const std::regex CARD_NAME_REGEX("\\$\\{([" + COMMON_CAPTURE + "]+)\\}|[" + COMMON_CAPTURE + "]+");

const char YIELD_UNTIL_TURN_KEY = 'y';
const char SKIP_STACK_KEY = 's';
const char CANCEL_YIELD_KEY = 'c';

using Client = client_interface<PollType>;

struct MessageMember {
    string message;
    bool isCard;
};


class ClientWrapper : public Window {
private:
    int _myID;
    bool _waitingResponse = false;
    PollType _lastRequestType;

    vector<string> _simpleChoices;
    vector<std::vector<MessageMember>> _simpleChoiceTextures;
    int _maxSimpleChoiceLength = 0;

    std::vector<MessageMember> _lastText;

    int _cardAmount = 0;
    vector<int> _chosenCardIDs;

    vector<int> _allowedCards;
    vector<int> _allowedPlayers;
    vector<int> _allowedStackMembers;
    
    string _host;
    Client* _c;
    MatchState _state;
    bool _hasState = false;
    bool _mouseLock = false;
    bool _mouseCheck = true;

    SDL_Event* _event = new SDL_Event();
    bool _running = false;

    bool _yield = false;
    bool _skipStack = false;
    int _lastTurnCounter = -1;
public:
    ClientWrapper(string host, string title, string assetsPath, bool fullscreen) :
        Window(title, assetsPath, fullscreen),
        _host(host)
    {
        clearSimpleChoiceTextures();
    }

    ~ClientWrapper() {
        delete _c;
        clearLastText();
    }

    void drawMessage(int x, int y, const std::vector<MessageMember>& message, int fontSize, SDL_Color textColor, SDL_Color cardColor) {
        auto spaceTex = _assets->getMessage(" ", SDL_Color{0, 0, 0, 0}, fontSize);
        auto spaceWidth = getSize(spaceTex).first;
        SDL_DestroyTexture(spaceTex);
        for (const auto& member : message) {
            auto tex = _assets->getMessage(member.message, member.isCard ? cardColor : textColor, fontSize);
            auto size = getSize(tex);
            int w = size.first;
            int h = size.second;
            this->drawTexture(tex, x, y);
            SDL_DestroyTexture(tex);
            int xx = x;
            x += size.first + spaceWidth;
            if (!member.isCard) continue;
            int mx, my;
            auto s = SDL_GetMouseState(&mx, &my);
            if (!(mx >= xx && my >= y && mx <= xx + w && my <= h + y)) continue;
            tex = this->_assets->getCard(member.message, CardSize::LARGE);
            size = getSize(tex);
            this->drawTexture(tex, _stackX - size.first, 0);
        }
    }


    void getActionCalc() {
        PlayerBoardState me;
        for (const auto& board : _state.boards)
            if (board.id == _myID)
                me = board;
        // check character card
        if (me.playerCard.active) _allowedCards.push_back(me.playerCard.id);
        // check board
        for (const auto& card : me.board) {
            //  TODO if tapped, go through all abilities, if at least one of them is not marked "requiresTap", add to allowed cards
            if (card.activatedAbilities.size()) 
                _allowedCards.push_back(card.id);
        }
        // check loot cards
        if (me.playableCount) {
            for (const auto& value : me.hand){
                _allowedCards.push_back(value.id);
            }
        }
        // check treasure
        if (me.purchaseCount && !_state.isCombat && me.coinCount >= me.treasurePrice && _state.stack.size() == 0)
            for (const auto& value : _state.shop)
                _allowedCards.push_back(value.id);
        // check monsters
        if (!_state.isCombat && _state.stack.size() == 0) {
            for (int i = 0; i < me.allowedAttackIndices.size(); i++) {
                auto index = me.allowedAttackIndices[i];
                if (index == -1) continue;
                if (_state.monsterDataArr[index].canBeAttacked)
                    _allowedCards.push_back(_state.monsters[index].id);
            }
        }
    }

    void clearSimpleChoiceTextures() {
        _simpleChoiceTextures.clear();
    }

    std::vector<MessageMember> parseMessage(string text) {
        std::vector<MessageMember> result;
        auto words_begin = std::sregex_iterator(text.begin(), text.end(), CARD_NAME_REGEX);
        auto words_end = std::sregex_iterator();
        for (auto i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            string message = match.str();
            MessageMember mem;
            mem.message = message;
            mem.isCard = false;
            if (match.str(1).size()) {
                mem.message = match.str(1);
                mem.isCard = true;
            }
            result.push_back(mem);
        }
        return result;
    }

    void setTopText(string text) {
        clearLastText();
        _lastText = parseMessage(text);
    }

    void clearLastText() {
        _lastText.clear();
    }

    void promptCalc(nlohmann::json j) {
        std::vector<int> choices;
        for (const auto& [key, value] : j["prompt"]["choices"].items()) choices.push_back(value);
        // last text texture
        setTopText(j["prompt"]["text"]);
        auto lastChoiceType = j["prompt"]["choiceType"];
        if (lastChoiceType == PLAYER_TARGET) {
            _allowedPlayers = choices;
            return;
        }
        if (lastChoiceType == CARD_TARGET) {
            _allowedCards = choices;
            return;
        }
        if (lastChoiceType == SHOP_CARD_TARGET) {
            _allowedCards = choices;
            return;
        }
        if (lastChoiceType == STACK_MEMBER_TARGET) {
            _allowedStackMembers = choices;
            return;
        }
        if (lastChoiceType == MONSTER_TARGET) {
            _allowedCards = choices;
            return;
        }
        if (lastChoiceType == SOUL_TARGET) {
            _allowedCards = choices;
            return;
        }
        //  TODO items in shop
    }

    void simplePromptCalc(nlohmann::json j) {
        setTopText(j["prompt"]["text"]);
        _maxSimpleChoiceLength = 0;
        for (const auto& [key, value] : j["prompt"]["choices"].items()) {
            _simpleChoices.push_back(value);
            auto message = parseMessage(value);
            _simpleChoiceTextures.push_back(message);
            auto width = getMessageWidth(message, 48);
            if (width > _maxSimpleChoiceLength) _maxSimpleChoiceLength = width;
        }
        _maxSimpleChoiceLength += 4;
    }

    int getMessageWidth(const std::vector<MessageMember>& message, int fontSize) {
        int result = 0;
        auto spaceTex = _assets->getMessage(" ", SDL_Color{0, 0, 0, 0}, fontSize);
        auto spaceWidth = getSize(spaceTex).first;
        SDL_DestroyTexture(spaceTex);
        for (const auto& member : message) {
            auto tex = _assets->getMessage(member.message, SDL_Color{0, 0, 0, 0}, fontSize);
            auto size = getSize(tex);
            SDL_DestroyTexture(tex);
            result += size.first + spaceWidth;
        }
        result -= spaceWidth;
        return result;
    }

    void drawSimplePromptBox() {
        if (!_simpleChoices.size()) return;
        int y = _boardStart + 1;
        int x = _sideBoardX + 1;
        auto fontSize = 48;
        this->drawRect(x, y, _maxSimpleChoiceLength, fontSize * _simpleChoices.size(), SDL_Color{255, 255, 255, 0}, true);
        int ci = -1;
        y -= fontSize;
        for (const auto& choice : _simpleChoiceTextures) {
            y += fontSize;
            ++ci;
            drawMessage(x, y, choice, fontSize, SDL_Color{0, 0, 0}, SDL_Color{0, 0, 255});
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
            clearLastText();
            _waitingResponse = false;
            _mouseLock = true;
        }
    }

    void chooseCardsCalc(nlohmann::json j) {
        setTopText(j["prompt"]["text"]);
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
        if (_lastRequestType == PollType::UpdateWinner) {
            auto winnerID = j["winnerID"];
            string message = "";
            SDL_Color color;
            if (winnerID == _myID) {
                message = "You won!";
                color = SDL_Color{0, 255, 0, 0};
            } else {
                string winnerName = "_err_";
                for (const auto& board : _state.boards)
                    if (board.id == winnerID)
                        winnerName = board.name;
                message = "You lost! (Winner: " + winnerName + ")";
                color = SDL_Color{255, 0, 0, 0};
            }
            setTopText(message);
        }
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
            if (_skipStack) respondPass();
            if (_yield) respondPass();
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
                if (msg.header.id == PollType::Setup) {
                    _myID = j["id"];
                    // for (const auto& [key, value] : j["cards"].items()){
                    //     _assets->createCard(value["name"], value["text"]);
                    // }
                    // load all cards
                    continue;
                }
                _state = MatchState(j);
                if (!_state.stack.size()) _skipStack = false;
                if (_yield && _state.currentID == _myID && _lastTurnCounter != _state.turnCounter) _yield = false;
                _hasState = true;
                _lastRequestType = msg.header.id;
                this->calcAllowedCards(j);
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
        case 'p':
            this->playFunnySound();
            break;
        case SDLK_SPACE:
            if (!_waitingResponse || _lastRequestType != PollType::GetAction) return;
            respondPass();
            break;
        case YIELD_UNTIL_TURN_KEY:
            _yield = true;
            _lastTurnCounter = _state.turnCounter;
            if (_lastRequestType != PollType::GetAction) return;
            if (_waitingResponse) respondPass();
            break;
        case SKIP_STACK_KEY:
            if (_state.stack.size()) {
                _skipStack = true;
                if (_lastRequestType != PollType::GetAction) return;
                if (_waitingResponse) respondPass();
            }
            break;
        case CANCEL_YIELD_KEY:
            _skipStack = false;
            _yield = false;
            break;
        }
    }

    void respondPass() {
        message<PollType> reply;
        reply << ACTION_PASS;
        _c->Send(reply);
        _waitingResponse = false;
    }

    void playFunnySound() {
        auto sound = new Sound("assets/sound/choir.wav");
        sound->setup();
        sound->play();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sound->stop();
        delete sound;
    }

    void sendAction(PlayerBoardState& state) {
        message<PollType> reply;
        reply << std::to_string(state.id);
        _c->Send(reply);
        clearLastText();
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
        clearLastText();
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

    string buyTreasureReply(CardState& card) {
        for (int i = 0; i < _state.shop.size(); i++) {
            if (card.id == _state.shop[i].id) {
                return string("buy_treasure ") + std::to_string(i);
            }
        }
        throw std::runtime_error("can't decide index of treasure " + card.cardName + " (id: " + std::to_string(card.id) + ")");
    }

    string attackMonsterReply(CardState& card) {
         for (int i = 0; i < _state.monsters.size(); i++) {
            if (card.id == _state.monsters[i].id) {
                return string("attack ") + std::to_string(i);
            }
        }
        throw std::runtime_error("can't decide index of monster " + card.cardName + " (id: " + std::to_string(card.id) + ")");
    }

    string activateCardReply(CardState& card) {
        auto result = "activate " + std::to_string(card.id) + " ";
        int resultI = 0;
        if (card.activatedAbilities.size() == 1)
            return result + std::to_string(resultI);
        std::vector<SDL_Texture*> buttons;
        for (const auto& effect : card.activatedAbilities)
            buttons.push_back(_assets->getMessage(effect.text, SDL_Color{0, 0, 0, 0}, 24));
        int x, y;
        auto s = SDL_GetMouseState(&x, &y);
        bool flag = true;
        bool mousePressed = false;
        int boxW = 0;
        for (const auto& tex : buttons) {
            auto w = getSize(tex).first;
            if (w > boxW) boxW = w;
        }
        int boxH = buttons.size() * 24;
        auto drawF = [&]() {
            this->drawRect(x, y, boxW, boxH, SDL_Color{255, 255, 255, 0}, true);
            for (int i = 0; i < buttons.size(); i++) {
                int tx = x;
                int ty = y + i * 24;
                this->drawTexture(buttons[i], x, ty);
                if (_mouseLock) continue;
                int mx, my;
                int ms = SDL_GetMouseState(&mx, &my);
                if (!(mx > tx && my > ty && mx < tx + boxW && my < ty + 24)) continue;
                this->drawRect(tx, ty, boxW, 24, SDL_Color{0, 0, 255, 0}, false);
                if (!mousePressed) continue;
                this->_mouseLock = true;
                resultI = i;
                flag = false;
                return;
            }
        };
        auto clearF = [&]() {
            this->drawRect(x, y, boxW, boxH, SDL_Color{255, 255, 255, 0}, true);
        };
        _mouseLock = true;
        while (flag) {
            while (SDL_PollEvent(this->_event)) {
                switch (_event->type) {
                case SDL_QUIT:
                    flag = false;
                    this->_running = false;
                    break;
                case SDL_KEYDOWN:
                    this->handleKey(_event->key.keysym.sym);
                    break;
                case SDL_MOUSEBUTTONUP:
                    _mouseLock = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousePressed = true;
                }
            }
            _mouseCheck = false;
            this->clear();
            this->draw();
            drawF();
            this->flush();
            mousePressed = false;
        }
        for (const auto& tex : buttons) {
            SDL_DestroyTexture(tex);
        }
        return result + std::to_string(resultI);
    }

    void sendAction(CardState& card) {
        message<PollType> reply;
        switch (card.zone) {
        case Zones::Unknown:
            throw std::runtime_error("Card " + card.cardName + " [" + std::to_string(card.id) + "] has unknown zone");
            return;
        case Zones::Hand:
            reply << "play_loot " + std::to_string(card.id);
            break;
        case Zones::Board:
            reply << activateCardReply(card);
            break;
        case Zones::CCard:
            reply << ACTION_ACTIVATE_CHARACTER_CARD;
            break;
        case Zones::Shop:
            reply << buyTreasureReply(card);
            break;
        case Zones::Stack:
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
            reply << attackMonsterReply(card);
            break;
        default:
            //  TODO
            break;
        }
        _c->Send(reply);
    }
    
    void answerPrompt(CardState& card) {
        message<PollType> reply;
        reply << std::to_string(card.id);
        _c->Send(reply);
        clearLastText();
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
        clearLastText();
        _waitingResponse = false;
    }

    void onClick(CardState card) {
        if (!_waitingResponse) return;
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

    void drawCard(CardState& card, int angle, int x, int y, Rect * bBox = nullptr, bool activatable = true) override {
        if (card.zone == Zones::Hand) {
            bool flag = false;
            for (const auto& aid : _allowedCards)
                if (card.id == aid) flag = true;
            if (!flag && card.ownerID != _myID) {
                drawCardBack("loot", true, x, y);
                return;
            }
        }
        Window::drawCard(card, angle, x, y, bBox);
        if (!_mouseCheck) return;
        if (!activatable && card.zone == Zones::Stack) return;
        if (_mouseLock) return;
        bool allowed = false;
        for (const auto& id : _allowedCards)
            if (id == card.id) allowed = true;
        if (!allowed) return;
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        int cardWidth = _cardSize.first;
        int cardHeight = _cardSize.second;
        if (bBox) {
            cardWidth = bBox->w;
            cardHeight = bBox->h;
            x = bBox->x;
            y = bBox->y;
        }
        int w = (angle == 0) ? cardWidth : cardHeight;
        int h = (angle == 0) ? cardHeight : cardWidth;
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

    void draw() {
        Window::draw(_state);
        if (_mouseCheck){
            // draw top text
            
            int x = _sideBoardX;
            int y = 0;
            drawMessage(x, y, _lastText, 48, SDL_Color{255, 0, 0}, SDL_Color{0, 255, 0});
            // draw simple choice choices
            this->drawSimplePromptBox();
        }
        _mouseCheck = true;
    }

    std::pair<int, int> drawPlayerName(PlayerBoardState& pboard, int x, int y) override {
        auto pair = Window::drawPlayerName(pboard, x, y);
        if (!_mouseCheck) return pair;
        int w = pair.first;
        int h = pair.second;
        bool flag = false;
        for (const auto& id : _allowedPlayers)
            if (id == pboard.id)
                flag = true;
        if (!flag) return pair;
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        if (mx >= x && my >= y && mx <= x + w && my <= h+ y) {
            auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 200};
            this->drawRect(x, y, w, h, color, false);
            this->drawRect(x+1, y+1, w-2, h-2, color, false);
            if (!_mouseLock && s&1) {
                onClick(pboard);
                _mouseLock = true;                
            }
        }
        return pair;
    }

    void drawSpecialStackMember(MatchState& state, int stackI, int x, int y) override {
        Window::drawSpecialStackMember(state, stackI, x, y);
        if (!_mouseCheck) return;
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

    void drawTreasureDeck(MatchState& state) override {
        Window::drawTreasureDeck(state);
        if (!_mouseCheck) return;
        PlayerBoardState* me;
        for (auto& board : _state.boards)
            if (board.id == _myID)
                me = &board;
        if (!_waitingResponse) return;
        if (_lastRequestType != PollType::GetAction) return;
        if (!(me->purchaseCount && !_state.isCombat && me->coinCount >= me->treasurePrice && _state.stack.size() == 0)) return;
        if (_mouseLock) return;
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        int w = _cardSize.first;
        int h = _cardSize.second;
        auto x = _treasureDeckX;
        auto y = _treasureDeckY;
        if (!(mx >= x && my >= y && mx <= x + w && my <= h+ y)) return;
        auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 0};
        this->drawRect(x, y, w, h, color, false);
        this->drawRect(x+1, y+1, w-2, h-2, color, false);
        if (!(s&1)) return;

        message<PollType> reply;
        reply << string("buy_treasure -1");
        _c->Send(reply);
        _mouseLock = true;
        _waitingResponse = false;
    }

    void drawMonsterDeck(MatchState& state) override {
        Window::drawMonsterDeck(state);
        if (!_mouseCheck) return;
        PlayerBoardState* me;
        for (auto& board : _state.boards)
            if (board.id == _myID)
                me = &board;
        if (!_waitingResponse) return;
        if (_lastRequestType != PollType::GetAction) return;
        if (!(me->allowedAttackIndices.size() && !_state.isCombat && _state.stack.size() == 0)) return;
        if (_mouseLock) return;
        bool flag = false;
        for (const auto& id : me->allowedAttackIndices) {
            if (id == -1) {
                flag = true;
                break;
            }
        }
        if (!flag) return;
        int mx, my;
        auto s = SDL_GetMouseState(&mx, &my);
        int w = _cardSize.first;
        int h = _cardSize.second;
        auto x = _monsterDeckX;
        auto y = _monsterDeckY;
        if (!(mx >= x && my >= y && mx <= x + w && my <= h+ y)) return;
        auto color = (s&1) ? SDL_Color{255, 0, 0, 0} : SDL_Color{0, 255, 0, 0};
        this->drawRect(x, y, w, h, color, false);
        this->drawRect(x+1, y+1, w-2, h-2, color, false);
        if (!(s&1)) return;

        message<PollType> reply;
        reply << string("attack -1");
        _c->Send(reply);
        _mouseLock = true;
        _waitingResponse = false;

    }
    
};

int main(int argc, char* argv[]) {
    // throw std::runtime_error(string(argv[0]));
    if (argc == 1 || argc > 3) {
        std::cout << "Invalid argument number" << std::endl;
        return 0;
    }
    string assetsPath = string(argv[1]);

    // string assetsPath = "@executable_path/.";
    string host = "localhost";
    if (argc == 3) host = string(argv[2]);
    ClientWrapper wrapper(host, "four-souls-client", assetsPath, false);
    wrapper.start();
    return 0;
}