Stack = {}
function Stack:Create()
    local t = {}
    t._et = {}

    function t:push(...)
        if ... then
            local targs = {...}
            for _, v in ipairs(targs) do
                table.insert(self._et, v)
            end
        end
    end
    function t:top()
        return t._et[#t._et]
    end
    function t:pop(num)
        local num = num or 1
        local entries = {}
        for i = 1, num do
            if #self._et ~= 0 then
                table.insert(entries, self._et[#self._et])
                table.remove(self._et)
            else
                break
            end
        end
        return table.unpack(entries)
    end
    function t:getn()
        return #self._et
    end
    function t:list()
        for i, v in pairs(self._et) do
            print(i, v)
        end
    end
    return t
end

-- end of turn stack

EOTStack = Stack:Create()

function Common_PopEOT(host)
    local top = EOTStack:pop()
    top[1](host, top[4], top[3])
end

function Common_PushEOT(host, func, cardID, ownerID, id)
    EOTStack:push({func, cardID, ownerID, id})
    deferEOT(host, cardID, 'Common_PopEOT', false)
end

-- func overriding stack

FuncStack = {}

function FuncStack:Create()
    local result = Stack:Create()
    function result:posOf(key)
        for pos, pair in ipairs(result._et) do
            if pair.name == key then
                return pos
            end
        end
    end
    
    function result:remove(key)
        table.remove(result._et, result:posOf(key))
    end
    return result
end

--  death stack effects
DeathStack = FuncStack:Create()

DeathStack:push({
    name = 'base',
    func = _deathPenalty
})

_deathPenalty = function (host, player)
    return DeathStack:top().func(host, player)
end

-- damage func stack
DamageFuncStack = FuncStack:Create()

DamageFuncStack:push({
    name = 'base',
    func = dealDamage
})

dealDamage = function (host, srcType, srcID, tgtType, tgtID, amount)
    DamageFuncStack:top().func(host, srcType, srcID, tgtType, tgtID, amount)
end

-- destroy card func stack
DestroyFuncStack = FuncStack:Create()

DestroyFuncStack:push({
    name = 'base',
    func = destroyCard
})

destroyCard = function (host, cardID)
    DestroyFuncStack:top().func(host, cardID)
end

-- gain treasure func stack
GainTreasureFuncStack = FuncStack:Create()

GainTreasureFuncStack:push({
    name = 'base',
    func = gainTreasure
})

gainTreasure = function (host, pid, amount)
    GainTreasureFuncStack:top().func(host, pid, amount)
end

-- loot cards func stack
LootCardsFuncStack = FuncStack:Create()

LootCardsFuncStack:push({
    name = 'base',
    func = lootCards
})

lootCards = function (host, pid, amount)
    LootCardsFuncStack:top().func(host, pid, amount)
end

-- recharge func stack
RechargeFuncStack = FuncStack:Create()

RechargeFuncStack:push({
    name = 'base',
    func = rechargeCard
})

rechargeCard = function (host, cid)
    RechargeFuncStack:top().func(host, cid)
end

-- layers
Layers = {}
function Layers:Create()
    local t = Stack:Create()
    function t:posOf(id)
        for pos, pair in ipairs(t._et) do
            if pair.id == id then
                return pos
            end
        end
        return 0
    end
    function t:remove(id)
        table.remove(t._et, t:posOf(id))
    end

    return t
end

-- health
MaxHealthLayers = Layers:Create()

MaxHealthLayers:push({
    id = 1,
    func = _getMaxHealth
})

_getMaxHealth = function (host, pid)
    return MaxHealthLayers:top().func(host, pid)
end

function Common_IncMaxLife(host, ownerID, value)
    local id = MaxHealthLayers:top().id + 1
    MaxHealthLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == ownerID then
                    add = value
                end
                return add + MaxHealthLayers._et[MaxHealthLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    healPlayer(host, ownerID, value)
    return id
end

function Common_DecMaxLife(host, id, ownerID)
    MaxHealthLayers:remove(id)
    for index, value in ipairs(MaxHealthLayers._et) do
        print(index, value.id)
    end
    healPlayer(host, ownerID, 0) -- a trick for resetting player's health if it's greater than max
end

function Common_TempIncMaxLife(host, cardID, targetID, amount)
    local id = Common_IncMaxLife(host, targetID, amount)
    -- print('NEW ID TO REMOVE: '..id)
    Common_PushEOT(host, Common_DecMaxLife, cardID, targetID, id)
    -- print('EOT SIZE: '..#EOTStack._et)
end

-- attack
AttackLayers = Layers:Create()

AttackLayers:push({
    id = 1,
    func = _getAttack
})

_getAttack = function (host, pid)
    return AttackLayers:top().func(host, pid)
end

function Common_IncAttack(ownerID, value)
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == ownerID then
                    add = value
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    return id
end

function Common_DecAttack(host, id, ownerID)
    AttackLayers:remove(id)
end

function Common_TempIncAttack(host, cardID, targetID, amount)
    local id = Common_IncAttack(targetID, amount)
    Common_PushEOT(host, Common_DecAttack, cardID, targetID, id)
end

-- monster roll layers
MonsterRollLayers = Layers:Create()

MonsterRollLayers:push({
    id = 1,
    func = _getMRoll
})

_getMRoll = function (host, mid)
    return MonsterRollLayers:top().func(host, mid)
end

function Common_IncMonsterRolls(ownerID, value)
    local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local p = getCurrentPlayer(host_)
                local add = 0
                if p.id == ownerID then
                    add = value
                end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
    return id
end

function Common_DecMonsterRolls(host, id, ownerID)
    MonsterRollLayers:remove(id)
end

-- monster power layers
MonsterPowerLayers = Layers:Create()

MonsterPowerLayers:push({
    id = 1,
    func = _getMPower
})

_getMPower = function (host, mid)
    return MonsterPowerLayers:top().func(host, mid)
end

-- combat damage layers
CombatDamageLayers = Layers:Create()

CombatDamageLayers:push({
    id = 1,
    func = _dealCombatDamageF
})

_dealCombatDamageF = function (host, srcType, srcID, tgtType, tgtID, amount, roll)
    return CombatDamageLayers:top().func(host, srcType, srcID, tgtType, tgtID, amount, roll)
end

-- treasure / loot

CardData = {}

function Common_ChooseMonster(host, ownerID)
    local monsters = getActiveMonsters(host)
    local ids = {}
    for i, p in ipairs(monsters) do
        ids[i] = p["id"]
    end
    local choiceId, _ = requestChoice(host, ownerID, "Choose a monster", MONSTER, ids)
    return Common_MonsterWithID(host, choiceId)
end

function Common_ChoosePlayer(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    for i, p in ipairs(players) do
        ids[i] = p["id"]
    end
    local choiceId, _ = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    return choiceId
end

function Common_ChooseOpponent(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    for _, p in ipairs(players) do
        if p.id ~= ownerID then
            ids[#ids+1] = p.id
        end
    end
    local choiceId, _ = requestChoice(host, ownerID, "Choose an opponent", PLAYER, ids)
    return choiceId
end

function Common_TrinketEnter( host )
    local me = this(host)
    local owner = getTopOwner(host)
    moveToBoard(host, owner.id, me.id)
end

function Common_TargetPlayer(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    for _, player in ipairs(players) do
        ids[#ids+1] = player.id
    end
    local choiceId, payed = requestChoice(host, ownerID, 'Choose a player', PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    return true
end

function Common_TargetLivingPlayer(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    for _, player in ipairs(players) do
        if not player.isDead then
            ids[#ids+1] = player.id
        end
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    return true
end

function Common_PlayerWithID(host, id)
    local players = getPlayers(host)
    for _, p in pairs(players) do
        if p.id == id then
            return p
        end
    end
end

function Common_Haunt_trigger_check(host, me)
    return Common_OwnerDied(host, me.ownerID)
end

function Common_Haunt_trigger_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo.ownerID)
end

function Common_Haunt_trigger(host)
    local me = this(host)
    local target = popTarget(host)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, target.id, me.id)
end


function Common_AttackingPlayerRolled(host, me, amount)
    local lr = Common_LastRoll(host, me)
    local flag = lr.value == amount and lr.isCombatRoll
    if not flag then
        return false
    end
    if not CardData[me.id] then
        CardData[me.id] = {}
    end
    CardData[me.id].pid = lr.ownerID
    return Common_MonsterWithID(host, me.id).isBeingAttacked
end

function Common_Pay(host, ownerID, amount)
    local player = Common_PlayerWithID(host, ownerID)
    if player.coins < amount then
        return false
    end
    subCoins(host, ownerID, amount)
    return true
end

function Common_CardCount(player)
    local count = 0
    for _ in pairs(player["hand"]) do count = count + 1 end
    return count
end

function Common_TargetMonsterOrPlayer(host, ownerID)
    local owner = Common_PlayerWithID(host, ownerID)
    local choice1 = 'Player'
    local choice2 = 'Monster'
    local choice = requestSimpleChoice(host, owner.id, 'Deal damage to who?', {choice1, choice2})
    if choice == choice1 then
        local target = Common_ChoosePlayer(host, owner.id)
        pushTarget(host, target, PLAYER)
        return true
    end
    local monster = Common_ChooseMonster(host, owner.id)
    pushTarget(host, monster.id, MONSTER)
    return true
end

function Common_TargetOpponent(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    local realI = 1
    for _, p in pairs(players) do
        if p["id"] ~= ownerID then
            ids[realI] = p["id"]
            realI = realI + 1
        end
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    return true
end

function Common_IsCombat(host)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        if monster.isBeingAttacked then
            return true
        end
    end
    return false
end

function Common_CardOnBoard(host, cid)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        for _, card in ipairs(player.board) do
            if card.id == cid then
                return true
            end
        end
    end
    return false
end

function Common_NotOnBoard(host, cid)
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if card.id == cid then
                return false
            end
        end
    end
    return true
end

function Common_SacrificeMe(host, cid)
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if card.id == cid then
                destroyCard(host, cid)
                return Common_NotOnBoard(host, cid)
            end
        end
    end
    return false
end

function Common_TargetAnySoul(host, ownerID)
    local cid, pid, chosen = Common_ChooseAnySoul(host, ownerID)
    if not chosen then
        return false
    end
    pushTarget(host, cid, SOUL)
    return true
end

function Common_ChooseAnySoul(host, ownerID)
    local ids = {}
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        for _, card in ipairs(player.souls) do
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return 0, 0, false
    end
    local choiceId, _ = requestChoice(host, ownerID, 'Choose a soul card', SOUL, ids)
    local pid = 0
    for _, player in ipairs(players) do
        for _, card in ipairs(player.souls) do
            if card.id == choiceId then
                pid = player.id
                break
            end
        end
    end
    return choiceId, pid, true
end

function Common_ChooseSoul(host, playerID, ownerID)
    local ids = {}
    local owner = Common_PlayerWithID(host, ownerID)
    for _, card in ipairs(owner.souls) do
        ids[#ids+1] = card.id
    end
    if #ids == 0 then
        return 0, false
    end
    local choiceId, _ = requestChoice(host, playerID, 'Choose a soul card', SOUL, ids)
    return choiceId, true
end

function Common_CurseCost(host, info)
    return Common_TargetPlayer(host, info.ownerID)
end

function Common_CurseEnter(host)
    local me = this(host)
    local target = popTarget(host)
    addCurse(host, target.id, me.id)
    Common_CurseTail(host)
end

function Common_CurseDeathCheck(host, me)
    return Common_OwnerDied(host, me.ownerID)
end

function Common_CurseDeathEffect(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    discardMe(host, me.id)
end

function Common_RerollAny(host, ownerID)
    local cardIDs = {}
    local owner = Common_PlayerWithID(host, ownerID)
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            cardIDs[#cardIDs+1] = card.id
        end
    end
    local removeItem = function (item)
        for i, value in ipairs(cardIDs) do
            if item == value then
                table.remove(cardIDs, i)
                return
            end
        end
    end
    local results = {}
    while #cardIDs ~= 0 do
        local choice = requestSimpleChoice(host, ownerID, 'Reroll an item?', {'Yes', 'No'})
        if choice == 'No' then
            break
        end
        choice, _ = requestChoice(host, ownerID, 'Choose an item to reroll', CARD, cardIDs)
        removeItem(choice)
        results[#results+1] = choice
    end
    for _, id in ipairs(results) do
        Common_RerollItem(host, id)
    end
end

function Common_OrderTopCards(host, ownerID, deckType, amount)
    local cards = topCardsOf(host, deckType, amount)
    if #cards ~= amount then
        return
    end
    for _, card in ipairs(cards) do
        removeFromEverywhere(host, card.id)
    end
    local choices = Common_FormCardChoices(cards)
    local posOf = function (choice_)
        for i, c in ipairs(choices) do
            if c == choice_ then
                return i
            end
        end
        return 0
    end
    while #choices ~= 0 do
        local choice = requestSimpleChoice(host, ownerID, 'Choose a card to put on top', choices)
        local pos = posOf(choice)
        table.remove(choices, pos)
        local card = cards[pos]
        table.remove(cards, pos)
        placeOnTop(host, deckType, card.id)
    end
end

function Common_SoulCount(player)
    local result = 0
    for _, card in ipairs(player.souls) do
        result = result + card.soulCount
    end
    return result
end

function Common_Discard(host, ownerID, amount)
    --  TODO? keep the fizzling of ability if player doesn't have enough cards in hand
    local player = Common_PlayerWithID(host, ownerID)
    if #player.hand < amount then
        return false
    end
    local message = "Choose a card to discard"
    if amount > 1 then
        message = "Choose "..amount.." cards to discard"
    end
    local cardIDs = {}
    if #player.hand == amount then
        for _, card in ipairs(player.hand) do
            cardIDs[#cardIDs+1] = card.id
        end
    else
        cardIDs = requestCardsInHand(host, ownerID, ownerID, message, amount)
    end
    for _, cid in ipairs(cardIDs) do
        discardLoot(host, ownerID, cid)
    end
    return true
end

function Common_RollStackSize(host)
    return #getRollStack(host)
end

function Common_ModLastRoll(host, value)
    local rs = getRollStack(host)
    local roll = rs[#rs]
    if roll.value == 6 then
        return
    end
    setRollValue(host, #rs-1, roll.value + value)
end

function Common_SacrificeItem(host, ownerID)
    local player = Common_PlayerWithID(host, ownerID)
    local ids = {}
    for _, card in ipairs(player.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return false
    end
    local choice, _ = requestChoice(host, ownerID, 'Choose an item to sacrifice', CARD, ids)
    destroyCard(host, choice)
    return true
end

function Common_LastRoll(host, me)
    local rolls = getRollStack(host)
    local roll = rolls[#rolls]
    if CardData[me.id] == nil then
        CardData[me.id] = {}
    end
    CardData[me.id].lastRoll = roll
    return roll
end

function Common_OpponentDied(host, ownerID)
    local death = Common_LastDeath(host)
    return death.type == PLAYER and death.id ~= ownerID
end

function Common_OwnerDied(host, ownerID)
    local death = Common_LastDeath(host)
    return death.type == PLAYER and death.id == ownerID
end

function Common_OwnerRolled(host, me, value)
    local roll = Common_LastRoll(host, me)
    if roll.ownerID ~= me.ownerID then
        return false
    end
    return roll.value == value
end

function Common_RollWithID(host, id)
    local stack = getStack(host)
    local rollI = 0
    for i, member in ipairs(stack) do
        if member.type == ROLL then
            rollI = rollI + 1
        end
        if i == id+1 then
            break
        end
    end
    return getRollStack(host)[rollI]
end

function Common_TargetRoll(host, ownerID)
    local rolls = getRollStack(host)
    if #rolls == 0 then
        return false
    end
    local stack = getStack(host)
    local choices = {}
    local ci = 1
    for i, member in ipairs(stack) do
        if member.type == ROLL then
            choices[ci] = i-1
            ci = ci + 1
        end
    end
    if #choices == 0 then
        return false
    end

    local choice, payed = requestChoice(host, ownerID, "Choose a roll", STACK, choices)
    if not payed then
        return false
    end
    pushTarget(host, choice, ROLL)
    return true
end

function Common_TargetCombatRoll(host, ownerID)
    local rolls = getRollStack(host)
    if #rolls == 0 then
        return false
    end
    local stack = getStack(host)
    local choices = {}
    local rp = 1
    for i, member in ipairs(stack) do
        if member.type == ROLL then
            if rolls[rp].isCombatRoll then
                choices[#choices+1] = i-1
            end
            rp = rp + 1
        end
    end
    if #choices == 0 then
        return false
    end
    local choice, payed = requestChoice(host, ownerID, "Choose a roll", STACK, choices)
    if not payed then
        return false
    end
    pushTarget(host, choice, ROLL)
    return true
end

function Common_TargetNonCombatRoll(host, ownerID)
    local rolls = getRollStack(host)
    if #rolls == 0 then
        return false
    end
    local stack = getStack(host)
    local choices = {}
    local rp = 1
    for i, member in ipairs(stack) do
        if member.type == ROLL then
            if not rolls[rp].isCombatRoll then
                choices[#choices+1] = i-1
            end
            rp = rp + 1
        end
    end
    if #choices == 0 then
        return false
    end
    local choice, payed = requestChoice(host, ownerID, "Choose a roll", STACK, choices)
    if not payed then
        return false
    end
    pushTarget(host, choice, ROLL)
    return true
end

function Common_BonusMonsterTail(host, meID)
    for _, pile in ipairs(getMonsterPiles(host)) do
        if #pile == 0 then
            pushRefillMonsters(host)
        end
    end
    discardMe(host, meID)
end

function Common_CurseTail(host)
    for _, pile in ipairs(getMonsterPiles(host)) do
        if #pile == 0 then
            pushRefillMonsters(host)
            return
        end
    end
end

function Common_FormCardChoices(cards)
    local result = {}
    for i, card in ipairs(cards) do
        result[#result+1] = i..'- ${'..card.name..'}'
    end
    return result
end

function Common_IncAttackCount(host, ownerID, required)
    local newPool = {-1}
    for i, _ in ipairs(getActiveMonsters(host)) do
        newPool[#newPool+1] = i-1
    end
    addAttackOpportunity(host, ownerID, required, false, newPool)
end

function Common_RemoveCounter(host)
    local card = this(host)
    if card.counters == 0 then
        return false
    end
    removeCounters(host, card.id, 1)
    return true
end

function Common_RemoveCounters(host, amount)
    local card = this(host)
    if card.counters < amount then
        return false
    end
    removeCounters(host, card.id, amount)
    return true
end

function Common_LastDeath(host)
    local deathStack = getDeathStack(host)
    return deathStack[#deathStack]
end

function Common_RechargeAnItem(host, ownerID, haveToBeOwnedByOwner)
    local choice, chose = Common_ChooseTappedCard(host, ownerID, haveToBeOwnedByOwner)
    if not chose then
        return
    end
    rechargeCard(host, choice)
end

function Common_TargetTappedCard(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    local idI = 1
    for _, player in pairs(players) do
        local board = player["board"]
        for _, card in ipairs(board) do
            if card["tapped"] then
                ids[idI] = card["id"]
                idI = idI + 1
            end
        end
    end
    if #ids == 0 then
        return false
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a tapped card", CARD, ids)
    if not payed then
        return false
    end
    pushTarget(host, choiceId, CARD)
    return true
end

function Common_ChooseTappedCard(host, ownerID, haveToBeOwnedByOwner)
    local players = getPlayers(host)
    local cardIDs = {}
    for _, player in ipairs(players) do
        if player.id == ownerID or not haveToBeOwnedByOwner then
            for _, card in ipairs(player.board) do
                if card.tapped then
                    cardIDs[#cardIDs+1] = card.id
                end
            end
        end
    end
    if #cardIDs == 0 then
        return -1, false
    end
    local choiceId, _ = requestChoice(host, ownerID, "Choose a tapped card", CARD, cardIDs)
    return choiceId, true
end

function Common_PostDeathOwnerDied(host, cardID)
    local owner = getOwner(host, cardID)
    local death = getLastDeath(host)
    return death.type == PLAYER and death.id == owner.id
end

function Common_OncePerTurn(host, cardID)
    local turnC = getTurnCounter(host)
    if CardData[cardID] == nil then
        CardData[cardID] = {
            lastTurn = turnC
        }
        return true
    end
    if CardData[cardID].lastTurn == turnC then
        return false
    end
    CardData[cardID].lastTurn = turnC
    return true
end

function Common_OwnerDamaged(host, cardID)
    local owner = getOwner(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    if damageEvent.targetType == PLAYER and damageEvent.targetID == owner.id then
        if CardData[cardID] == nil then
            CardData[cardID] = {}
        end
        CardData[cardID].damageEvent = damageEvent
        return true
    end
    return false
end

function Common_MonsterDamaged(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    if damageEvent.targetType == MONSTER and damageEvent.targetID == cardID then
        if CardData[cardID] == nil then
            CardData[cardID] = {}
        end
        CardData[cardID].damageEvent = damageEvent
        return true
    end
    return false
end

function Common_MonsterDealtCombatDamage(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    if damageEvent.sourceType == MONSTER and damageEvent.sourceID == cardID and damageEvent.isCombatDamage then
        if CardData[cardID] == nil then
            CardData[cardID] = {}
        end
        CardData[cardID].damageEvent = damageEvent
        return true
    end
    return false
end

function Common_MonsterIsBeingAttacked(host, cardID)
    local monster = Common_MonsterWithID(host, cardID)
    return monster.isBeingAttacked
end

function Common_OwnerDealtDamage(host, cardID, targetType)
    local targetType = targetType or MONSTER
    local owner = getOwner(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    if damageEvent.sourceType ~= PLAYER then
        return false
    end
    if damageEvent.sourceID ~= owner.id then
        return false
    end
    if damageEvent.targetType ~= targetType then
        return false
    end
    local monster = Common_MonsterWithID(host, damageEvent.targetID)
    if not monster.isBeingAttacked then
        return false
    end
    return true
end

function Common_OwnerDealtCombatDamage(host, cardID, targetType)
    local targetType = targetType or MONSTER
    local owner = getOwner(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    if not damageEvent.isCombatDamage then
        return false
    end
    if damageEvent.sourceType ~= PLAYER then
        return false
    end
    if damageEvent.sourceID ~= owner.id then
        return false
    end
    if damageEvent.targetType ~= targetType then
        return false
    end
    -- local monster, found = Common_MonsterWithID(host, damageEvent.targetID)
    -- if not found then
    --     return false
    -- end
    -- print(monster.isBeingAttacked)
    -- if not monster.isBeingAttacked then
    --     return false
    -- end
    if CardData[cardID] == nil then
        CardData[cardID] = {}
    end
    CardData[cardID].damageEvent = damageEvent
    return true
end

function Common_OwnersTurn(host, cardID)
    local owner = getOwner(host, cardID)
    local currentPlayer = getCurrentPlayer(host)
    return owner.id == currentPlayer.id
end

function Common_AllPlayersLoot(host, amount)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        lootCards(host, player["id"], amount)
    end
end

function Common_AllPlayersAddCoins(host, amount)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        addCoins(host, player["id"], amount)
    end
end

function Common_DamageAllPlayers(host, amount)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        dealDamage(host, PLAYER, owner.id, PLAYER, player.id, amount)
    end
end

function Common_TargetMonster(host, ownerID)
    local monsters = getActiveMonsters(host)
    local ids = {}
    for i, p in ipairs(monsters) do
        ids[i] = p.id
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a monster", MONSTER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, MONSTER)
    return true
end

function Common_TargetNonAttackedMonster(host, ownerID)
    local monsters = getActiveMonsters(host)
    local ids = {}
    for _, card in ipairs(monsters) do
        if not card.isBeingAttacked then
            ids[#ids+1] = card.id
        end
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a monster", MONSTER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, MONSTER)
    return true
end

function Common_MonsterWithID(host, mid)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        if mid == monster.id then
            return monster, true
        end
    end
    return {}, false
end

function Common_popMonsterTarget(host)
    local target = popTarget(host)
    return Common_MonsterWithID(host, target["id"])
end

function Common_TargetOwnedNonEternalCard(host, ownerID)
    local cardIDs = {}
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if not card.isEternal then
                cardIDs[#cardIDs+1] = card.id
            end
        end
    end
    if #cardIDs == 0 then
        return false
    end
    local choice, _ = requestChoice(host, ownerID, 'Choose a non-eternal card', CARD, cardIDs)
    pushTarget(host, choice, CARD)
    return true
end

function Common_TargetNonEternalCard(host, ownerID)
    local cardIDs = {}
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if not card.isEternal then
                cardIDs[#cardIDs+1] = card.id
            end
        end
    end
    for _, card in ipairs(getShop(host)) do
        if not card.isEternal then
            cardIDs[#cardIDs+1] = card.id
        end
    end
    if #cardIDs == 0 then
        return false
    end
    local choice, _ = requestChoice(host, ownerID, 'Choose a non-eternal card', CARD, cardIDs)
    pushTarget(host, choice, CARD)
    return true
end

function Common_ChooseNonEternalCard(host, ownerID)
    local cardIDs = {}
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if not card.isEternal then
                cardIDs[#cardIDs+1] = card.id
            end
        end
    end
    if #cardIDs == 0 then
        return {}, false
    end
    local choice, _ = requestChoice(host, ownerID, 'Choose item', CARD, cardIDs)
    return choice, true
end

function Common_ChooseShopItem(host, ownerID)
    local ids = {}
    for _, card in ipairs(getShop(host)) do
        ids[#ids+1] = card.id
    end
    local choice, _ = requestChoice(host, ownerID, 'Choose a shop item', SHOP_CARD, ids)
    return choice
end

function Common_TargetShopItem(host, ownerID)
    pushTarget(host, Common_ChooseShopItem(host, ownerID), SHOP_CARD)
    return true
end

function Common_RerollItem(host, cardID)
    local owner = getOwner(host, cardID)
    if owner == nil then
        for _, card in ipairs(getShop(host)) do
            if card.id == cardID then
                destroyCard(host, cardID)
                pushRefillShop(host)
                return
            end
        end
        return
    end
    destroyCard(host, cardID)
    gainTreasure(host, owner.id, 1)
end

function Common_OwnerKilledMonster(host, cardID)
    local death = Common_LastDeath(host)
    if death.type == PLAYER then
        return false
    end
    local owner = getOwner(host, cardID)
    local killerID = getLastKillerID(host)
    return owner.id == killerID
end