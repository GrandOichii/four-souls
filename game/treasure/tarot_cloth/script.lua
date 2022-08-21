function TarotCloth_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 4
end

function TarotCloth_trigger(host)
    local lr = CardData[this(host).id].lastRoll
    local owner = getTopOwner(host)
    local player = Common_PlayerWithID(host, lr.ownerID)
    if #player.hand == 0 then
        return
    end
    local cardID = requestCardsInHand(host, owner.id, player.id, "Choose a card to give to "..owner.name, 1)[1]
    removeFromEverywhere(host, cardID)
    moveToHand(host, owner.id, cardID)
end