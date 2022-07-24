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

--  death stack effects
DeathStack = Stack:Create()

function DeathStack:posOf(key)
    for pos, pair in ipairs(DeathStack._et) do
        if pair.name == key then
            return pos
        end
    end
end

function DeathStack:remove(key)
    table.remove(DeathStack._et, DeathStack:posOf(key))
end

DeathStack:push({
    name = 'base',
    func = _deathPenalty
})

_deathPenalty = function (host, player)
    DeathStack:top().func(host, player)
end

-- health
MaxHealthLayers = Stack:Create()

function MaxHealthLayers:posOf(key)
    for pos, pair in ipairs(MaxHealthLayers._et) do
        if pair.name == key then
            return pos
        end
    end
end

function MaxHealthLayers:remove(key)
    table.remove(MaxHealthLayers._et, MaxHealthLayers:posOf(key))
end

MaxHealthLayers:push({
    name = 'base',
    func = _getMaxHealth
})

_getMaxHealth = function (host, pid)
    return MaxHealthLayers:top().func(host, pid)
end

function Common_IncMaxLife(cardID, ownerID, value)
    local key = cardID
    MaxHealthLayers:push(
        {
            name = key,
            func = function (host_, pid)
                local add = 0
                if pid == ownerID then
                    add = value
                end
                return add + MaxHealthLayers._et[MaxHealthLayers:posOf(key)-1].func(host_, pid)
            end
        }
    )
    --  TODO heal player
end

function Common_DecMaxLife(cardID)
    MaxHealthLayers:remove(cardID)
end

-- attack
AttackLayers = Stack:Create()

function AttackLayers:posOf(key)
    for pos, pair in ipairs(AttackLayers._et) do
        if pair.name == key then
            return pos
        end
    end
end

function AttackLayers:remove(key)
    table.remove(AttackLayers._et, AttackLayers:posOf(key))
end

AttackLayers:push({
    name = 'base',
    func = _getAttack
})

_getAttack = function (host, pid)
    return AttackLayers:top().func(host, pid)
end

function Common_IncAttack(cardID, ownerID, value)
    local key = cardID
    AttackLayers:push(
        {
            name = key,
            func = function (host_, pid)
                local add = 0
                if pid == ownerID then
                    add = value
                end
                return add + AttackLayers._et[AttackLayers:posOf(key)-1].func(host_, pid)
            end
        }
    )
    --  TODO heal player
end

function Common_DecAttack(cardID)
    AttackLayers:remove(cardID)
end

EOTStack = Stack:Create()

function Common_PopEOT(host)
    local top = EOTStack:pop()
    print('POPPED TOP: '..top[2])
    top[1](top[2])
end

function Common_PushEOT(host, func, cardID)
    EOTStack:push({func, cardID})
    deferEOT(host, cardID, 'Common_PopEOT', false)
end

function Common_TempIncMaxLife(host, cardID, targetID, amount)
    Common_IncMaxLife(cardID, targetID, amount)
    Common_PushEOT(host, Common_DecMaxLife, cardID)
end

function Common_TempIncAttack(host, cardID, targetID, amount)
    Common_IncAttack(cardID, targetID, amount)
    Common_PushEOT(host, Common_DecAttack, cardID)
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
    print('OWNER ID: '..ownerID)
    local ids = {}
    for _, p in ipairs(players) do
        if p.id ~= ownerID then
            ids[#ids+1] = p.id
        end
    end
    print('CHOICES:')
    for _, i in ipairs(ids) do
        print('\t'..i)
    end
    local choiceId, _ = requestChoice(host, ownerID, "Choose an opponent", PLAYER, ids)
    print('CHOICE ID:'..choiceId)
    return choiceId
end

function Common_TargetPlayer(host, cardInfo)
    local ownerID = cardInfo["ownerID"]
    local players = getPlayers(host)
    local ids = {}
    for i, p in ipairs(players) do
        ids[i] = p["id"]
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    return true
end

function Common_PlayerWithID(host, id)
    local players = getPlayers(host)
    for _, p in pairs(players) do
        if p["id"] == id then
            return p
        end
    end
end

function Common_Pay(host, ownerID, amount)
    local player = Common_PlayerWithID(host, ownerID)
    if player["coins"] < amount then
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

function Common_TargetOpponent(host, cardInfo)
    local ownerID = cardInfo["ownerID"]
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

function Common_Tap(host)
    local card = this(host)
    if card["tapped"] then
        return false
    end
    tapCard(host, card["id"])
    return true
end

function Common_Discard(host, ownerID, amount)
    local player = Common_PlayerWithID(host, ownerID)
    if #player["hand"] < amount then
        return false
    end
    local message = "Choose a card to discard"
    if amount > 1 then
        message = "Choose "..amount.." cards to discard"
    end
    local cardIDs = requestCardsInHand(host, ownerID, ownerID, message, amount)
    --  TODO debug
    for _, cid in ipairs(cardIDs) do
        print("DISCARDING CARD WITH ID "..cid)
        discardLoot(host, ownerID, cid)
    end
    return true
end

function Common_Roll(host, ownerID)
    pushRollEvent(host, ownerID)
    return true
end

function Common_RollStackSize(host)
    return #getRollStack(host)
end

function Common_LastRoll(host)
    local rolls = getRollStack(host)
    local roll = rolls[#rolls]
    return roll
end

function Common_OpponentDied(host, ownerID)
    local death = Common_LastDeath(host)
    return death["type"] == PLAYER and death["id"] ~= ownerID
end

function Common_OwnerDied(host, ownerID)
    local death = Common_LastDeath(host)
    return death.type == PLAYER and death.id == ownerID
end

function Common_OwnerRolled(host, ownerID, value)
    local roll = Common_LastRoll(host)
    if roll["ownerID"] ~= ownerID then
        return false
    end
    return roll["value"] == value
end

function Common_RollWithID(host, id)
    local stack = getStack(host)
    local rollI = 0
    for i, member in ipairs(stack) do
        if member["type"] == ROLL then
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
        if member["type"] == ROLL then
            choices[ci] = i-1
            ci = ci + 1
        end
    end
    local choice, payed = requestChoice(host, ownerID, "Choose a roll", STACK, choices)
    if not payed then
        return false
    end
    pushTarget(host, choice, ROLL)
    return true
end

function Common_RemoveCounter(host)
    local card = this(host)
    print("attempting to remove counters "..card["counters"])
    if card["counters"] == 0 then
        return false
    end
    removeCounters(host, card["id"], 1)
    return true
end

function Common_RemoveCounters(host, amount)
    for i = 1, amount do
        Common_RemoveCounter(host)
    end
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
    return  death["type"] == PLAYER and death["id"] == owner["id"]
end

function Common_OwnerDamaged(host, cardID)
    local owner = getOwner(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    return damageEvent["targetType"] == PLAYER and damageEvent["targetID"] == owner["id"]
end

function Common_OwnerDealtCombatDamage(host, cardID)
    local owner = getOwner(host, cardID)
    local damageEvent = getTopDamageEvent(host)
    local lastRoll = getLastRoll(host)
    -- print('LAST ROLL: VALUE: '..lastRoll.value..', IS COMBAT ROLL: '..lastRoll.isCombatRoll)
    if not lastRoll.isCombatRoll then
        return false
    end
    if damageEvent.sourceType ~= PLAYER then
        return false
    end
    print('DAMAGE SOURCE ID: '..damageEvent.sourceID..', MY ID: '..owner.id)
    if damageEvent.sourceID ~= owner.id then
        return false
    end
    if damageEvent.targetType ~= MONSTER then
        return false
    end
    local monster = Common_MonsterWithID(host, damageEvent.targetID)
    if not monster.isBeingAttacked then
        return false
    end
    return true
end

function Common_OwnersTurn(host, cardID)
    local owner = getOwner(host, cardID)
    local currentPlayer = getCurrentPlayer(host)
    return owner["id"] == currentPlayer["id"]
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
        dealDamage(host, PLAYER, owner["id"], PLAYER, player["id"], amount)
    end
end

function Common_TargetMonster(host, ownerID)
    local monsters = getActiveMonsters(host)
    local ids = {}
    for i, p in ipairs(monsters) do
        ids[i] = p["id"]
        print('mid: '..p['id'])
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a monster", MONSTER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, MONSTER)
    return true
end

function Common_MonsterWithID(host, mid)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        if mid == monster["id"] then
            return monster, true
        end
    end
    return {}, false
end

function Common_popMonsterTarget(host)
    local target = popTarget(host)
    return Common_MonsterWithID(host, target["id"])
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
    local choice, _ = requestChoice(host, ownerID, 'Choose card', CARD, cardIDs)
    return choice, true
end

function Common_RerollItem(host, cardID)
    local owner = getOwner(host, cardID)
    destroyCard(host, cardID)
    plusOneTreasure(host, owner.id)
end

function Common_OwnerKilledMonster(host, cardID)
    local death = Common_LastDeath(host)
    if death.type == PLAYER then
        return false
    end
    local owner = getOwner(host, cardID)
    local killerID = getLastKillerID(host)
    print('LAST KILLER ID: '..killerID)
    return owner.id == killerID
end