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

function FileExists(name)
    local f=io.open(name,"r")
    if f~=nil then io.close(f) return true else return false end
end

function WriteTable(table, path)
    local res = json.encode(table)
    local file = io.open(path, "w")
    io.output(file)
    io.write(res)
    io.close(file)
end

function ReadTable(path)
    local file = io.open(path, "r")
    local data = file:read("*all")
    io.close(file)
    local res = json.decode(data)
    return res
end

local DATA_PATH = 'bots/data.json'

if not FileExists(DATA_PATH) then
    WriteTable({}, DATA_PATH)
end

local ITEMS = {}
local WEIGHTS = ReadTable(DATA_PATH)

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

local function GetW(iName)
    WEIGHTS = ReadTable(DATA_PATH)
    for key, value in pairs(WEIGHTS) do
        if key == iName then
            return value
        end
    end
    WEIGHTS[iName] = 50
    WriteTable(WEIGHTS, DATA_PATH)
    return 50
end

local function AttemptBuyTreasure(me, state)
    if not (me.purchaseCount ~= 0 and me.coins >= me.treasurePrice) then
        return false, ''
    end
    local mV = 0
    local cI = -1
    local cName = ''
    for i, card in ipairs(state.shop) do
        local w = GetW(card.name)
        local v = math.random(1, w)
        if v > mV then
            mV = v
            cI = i-1
            cName = card.name
        end
    end
    ITEMS[#ITEMS+1] = cName
    print('\tBOUGHT '..cName)
    return true, 'buy_treasure '..cI
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
    local map = activationMap[trinket.id]
    if map == nil then
        activationMap[trinket.id] = {
            me = me,
            state = state,
        }
        return true
    end
    if isSameMe(map.me, me) or isSameState(map.state, state) then
        return false
    end
    activationMap[trinket.id] = {
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

local ME_ID = nil

--[[
0.25
0.5
0.75
]]--

-- new = cur + (cur / 2)
-- old = 

function Bot_PromptAction(me, state)
    if ME_ID == nil then
        ME_ID = me.id
    end
    -- print(json.encode(me))
    -- if me.id == state.currentID and state.isMain then
    --     -- main phase actions
    --     local played, message = AttemptPlayLoot(me, state)
    --     if played then
    --         return message
    --     end
    --     local purchased, message = AttemptBuyTreasure(me, state)
    --     if purchased then
    --         return message
    --     end
    --     local activated, message = AttemptActivate(me, state)
    --     if activated then
    --         return message
    --     end
    --     local attacked, message = AttemptAttack(me, state)
    --     if attacked then
    --         return message
    --     end
    --     return '$PASS'
    -- end
    -- responses
    return '$PASS'
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    return '$FIRST'
end

function Bot_PromptSimpleResponse(me, state, text, choices)
    return '$FIRST'
end

function Bot_UpdateWinner(winnerID)
    -- local wFunc = function (w)
    --     local r = w + 2
    --     if r > 100 then
    --         return 100
    --     end
    --     return r
    -- end
    -- if winnerID ~= ME_ID then
    --     wFunc = function (w)
    --         local r = w - 1
    --         if r < 1 then
    --             return 1
    --         end
    --         return r
    --     end
    -- end
    -- for _, iName in ipairs(ITEMS) do
    --     WEIGHTS[iName] = wFunc(WEIGHTS[iName])
    -- end
    -- WriteTable(WEIGHTS, DATA_PATH)
end