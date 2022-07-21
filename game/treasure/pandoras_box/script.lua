function PandorasBox_cost(host, cardInfo)
    local card = this(host)
    local isUntapped = Common_Tap(host)
    if not isUntapped then
        return false
    end
    Common_Roll(host, cardInfo.ownerID)
    CardData[card.id] = {
        prevOwnerID = cardInfo.ownerID
    }
    destroyCard(host, card.id)
    return true
end

function PandorasBox_tap(host)
    local card = this(host)
    local roll = getLastRoll(host).value
    local owner = Common_PlayerWithID(host, CardData[card.id].prevOwnerID)
    if roll == 1 then
        addCoins(host, owner.id, 1)
        return
    end
    if roll == 2 then
        addCoins(host, owner.id, 6)
        return
    end
    if roll == 3 then
        local monster = Common_ChooseMonster(host, owner.id)
        killEntity(host, MONSTER, monster.id)
        return
    end
    if roll == 4 then
        lootCards(host, owner.id, 3)
        return
    end
    if roll == 5 then
        addCoins(host, owner.id, 9)
        return
    end
    if roll == 6 then
        addSouls(host, owner.id, 1)
        return
    end
end