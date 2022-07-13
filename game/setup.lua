math.randomseed( os.time() )

function Common_TargetPlayer(host, cardInfo)
    local ownerID = cardInfo["ownerID"]
    local players = getPlayers(host)
    local ids = {}
    for i, p in pairs(players) do
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

function Common_Tap(host, cardInfo)
    local card = this(host)

    if card["tapped"] then
        return false
    end
    tapCard(host, card["id"])
    return true
end

function Common_Roll(host, cardInfo)
    pushRollEvent(host, cardInfo["ownerID"])
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
        return 0
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a card to untap", CARD, ids)
    if not payed then
        return false
    end
    pushTarget(host, choiceId, CARD)
    return true
end