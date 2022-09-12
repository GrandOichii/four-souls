function Ansuz_cost(host, info)
    local me = this(host)
    local choice = requestSimpleChoice(host, info.ownerID, 'Choose a deck', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    CardData[me.id] = {
        dChoice = choice
    }
    return true
end

function Ansuz_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, CardData[me.id].dChoice, 4)
    discardMe(host, me.id)
end
