#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include "common.hpp"
#include "window.hpp"

using Client = client_interface<PollType>;

class ClientWrapper : public Window {
private:
    int _myID;
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
    MatchState _state;
    bool _hasState = false;
    bool _mouseLock = false;

    SDL_Event* _event = new SDL_Event();
    bool _running = false;
public:
    ClientWrapper(string host, string title, string assetsPath, bool fullscreen) :
        Window(title, assetsPath, fullscreen),
        _host(host)
    {
        clearSimpleChoiceTextures();
    }

    ~ClientWrapper() {
        delete _c;
        SDL_DestroyTexture(_lastTextTex);
    }

    void getActionCalc() {
        //  TODO temporary fix for getting action when not having priority
        PlayerBoardState me;
        for (const auto& board : _state.boards)
            if (board.id == _myID)
                me = board;
        // check character card
        if (me.playerCard.active) _allowedCards.push_back(me.playerCard.id);
        // check board
        for (const auto& value : me.board)
            if (value.active) _allowedCards.push_back(value.id);
        // check loot cards
        std::cout << "CLIENT PLAYABLE COUNT " << me.playableCount << std::endl;
        if (me.playableCount)
            for (const auto& value : me.hand)
                _allowedCards.push_back(value.id);
        // check treasure
        if (me.purchaseCount && !_state.isCombat && me.coinCount >= me.treasurePrice && _state.stack.size() == 0)
            for (const auto& value : _state.shop)
                _allowedCards.push_back(value.id);
        // check monsters
        if (me.attackCount && _state.stack.size() == 0) {
            for (const auto& value : _state.monsters)
                _allowedCards.push_back(value.id);
        }
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
        if (lastChoiceType == MONSTER_TARGET) {
            _allowedCards = choices;
            return;
        }
        //  TODO items in shop
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
        this->drawRect(x, y, _maxSimpleChoiceLength, fontSize * _simpleChoices.size(), SDL_Color{255, 255, 255, 0}, true);
        int ci = -1;
        y -= fontSize;
        for (const auto& tex : _simpleChoiceTextures) {
            y += fontSize;
            ++ci;
            this->drawTexture(tex, x, y);
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
            std::cout << "WINNER " << winnerID << std::endl;
            _lastTextTex = _assets->getMessage(message, color, 24);
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
                    for (const auto& [key, value] : j["cards"].items()){
                        _assets->createCard(value["name"], value["text"]);
                    }
                    // load all cards
                    continue;
                }
                _state = MatchState(j);
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
        case SDLK_SPACE:
            if (!_waitingResponse || _lastRequestType != PollType::GetAction) return;
            message<PollType> reply;
            reply << ACTION_PASS;
            _c->Send(reply);
            _waitingResponse = false;
        }
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
            reply << buyTreasureReply(card);
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
            reply << attackMonsterReply(card);
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

    void drawCard(CardState& card, int angle, int x, int y) override {
        //  TODO hide opponents hand
        Window::drawCard(card, angle, x, y);
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

    void draw() {
        Window::draw(_state);
        // draw top text
        if (_lastTextTex) {
            this->drawTexture(_lastTextTex, _sideBoardX, 0);
        }
        // draw simple choice choices
        this->drawSimplePromptBox();
    }

    std::pair<int, int> drawPlayerName(PlayerBoardState& pboard, int x, int y) override {
        auto pair = Window::drawPlayerName(pboard, x, y);
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
            if (s&1) {
                onClick(pboard);
                _mouseLock = true;                
            }
        }
        return pair;
    }

    void drawSpecialStackMember(MatchState& state, int stackI, int x, int y) override {
        Window::drawSpecialStackMember(state, stackI, x, y);
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
    
};

int main(int argc, char* argv[]) {
    // string host, string title, string assetsPath, bool fullscreen
    if (argc == 1 || argc > 3) {
        std::cout << "Invalid argument number" << std::endl;
        return 0;
    }
    string assetsPath = string(argv[1]);
    string host = "localhost";
    if (argc == 3) host = string(argv[2]);
    ClientWrapper wrapper(host, "four-souls-client", assetsPath, false);
    wrapper.start();
    return 0;
}