math.randomseed( os.time() )

-- treasure / loot

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

function Common_LastDeath(host)
    local deathStack = getDeathStack(host)
    return deathStack[#deathStack]
end

function Common_TargetTappedCard(host, ownerID)
    local players = getPlayers(host)
    local ids = {}
    local idI = 1
    for _, player in pairs(players) do
        local board = player["board"]
        for _, card in ipairs(board) do
            if card["tapped"] then
                print("--> "..card["name"])
                ids[idI] = card["id"]
                idI = idI + 1
            end
        end
    end
    if #ids == 0 then
        return false
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a card to untap", CARD, ids)
    if not payed then
        return false
    end
    pushTarget(host, choiceId, CARD)
    return true
end

function Common_PostDeathOwnerDied(host, cardID)
    local owner = getOwner(host, cardID)
    local death = getLastDeath(host)
    return  death["type"] == PLAYER and death["id"] == owner["id"]
end

function Common_OwnerDamaged(host, cardID)
    local owner = getOwner(host, cardID)
    local damageEvent = getDamageEvent(host)
    return damageEvent["targetType"] == PLAYER and damageEvent["targetID"] == owner["id"]
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