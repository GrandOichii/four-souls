--[[
    killed a monster:
    get the monster's danger level = mp
    calculate the known items' power level = p
    p > mp
    yes:
        do nothing
    no:
        p = 0
        yes:
            divide the mp equally between items, end
        no:
                    

]]--

package.path = package.path..';lua/?.lua'
local json = require "json"

local function isSameState(state1, state2)
    return json.encode(state1) == json.encode(state2)
end

local function isSameMe(me1, me2)
    return json.encode(me1) == json.encode(me2)
end

local lootBlacklist = {
    'Dice Shard',
    'XX. Judgement',
    'Lil Battery',
    'I. The Magician',
    'Perthro'
}

local function IsBlacklistedLoot(card)
    for _, bcard in ipairs(lootBlacklist) do
        if card.name == bcard then
            return true
        end
    end
    return false
end

local function CalcBestCard(me, state)
    for _, card in ipairs(me.hand) do
        if not IsBlacklistedLoot(card) then
            return card, true
        end
    end
    return {}, false
end

local function AttemptPlayLoot(me, state)
    local lootCards = me.hand
    if #lootCards ~= 0 then
        if me.playableCount ~= 0 then
            local card, good = CalcBestCard(me, state)
            if good then
                return true, 'play_loot ' .. card.id
            end
        end
        if me.characterActive then
            return true, 'activate_character'
        end
    end
    return false, ''
end

local function AttemptBuyTreasure(me, state)
    if me.purchaseCount ~= 0 and me.coins >= me.treasurePrice then
        return true, 'buy_treasure -1'
    end
    return false, ''
end

local function ChooseBestToAttack(me, state)
    return 'attack 0'
end

local function AttemptAttack(me, state)
    if me.attackCount ~= 0 then
        return true, ChooseBestToAttack(me, state)
    end
    return false, ''
end

local activationMap = {}

local function canActivate(me, state, trinket)
    local map = activationMap[trinket['id']]
    if map == nil then
        activationMap[trinket['id']] = {
            me = me,
            state = state,
        }
        return true
    end
    if isSameMe(map.me, me) or isSameState(map.state, state) then
        return false
    end
    activationMap[trinket['id']] = {
        me = me,
        state = state,
    }
    return true
end

local function AttemptActivate(me, state)
    local trinkets = me.board
    for _, trinket in pairs(trinkets) do
        if not trinket.passive and not trinket.tapped then
            if canActivate(me, state, trinket) then
                return true, 'activate ' .. trinket.id .. ' 0'
            end
        end
    end
    return false, ''
end

function Bot_PromptAction(me, state)
    if me.id == state.currentID and state.isMain then
        -- main phase actions
        local played, message = AttemptPlayLoot(me, state)
        if played then
            return message
        end
        local purchased, message = AttemptBuyTreasure(me, state)
        if purchased then
            return message
        end
        local activated, message = AttemptActivate(me, state)
        if activated then
            return message
        end
        local attacked, message = AttemptAttack(me, state)
        if attacked then
            return message
        end
        return '$PASS'
    end
    -- responses
    return '$PASS'
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    return '$FIRST'
end

function Bot_PromptSimpleResponse(me, state, text, choices)
    return '$FIRST'
end