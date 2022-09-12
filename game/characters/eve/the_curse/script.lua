function TheCurse_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheCurse_trigger_cost(host, info)
    local me = this(host)
    --  TODO better text
    local choice = requestSimpleChoice(host, info.ownerID, 'Choose a deck to mill', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    CardData[me.id] = {
        dChoice = choice
    }
    return true
end

function TheCurse_trigger(host)
    local me = this(host)
    millDeck(host, CardData[me.id].dChoice, 1)
end

function TheCurse_cost(host, info)
    local me = this(host)
    local choice = requestSimpleChoice(host, info.ownerID, 'Choose discard', {'Treasure', 'Loot', 'Monster'})
    CardData[me.id] = {
        dChoice = choice
    }
    return true
end

function TheCurse_effect(host)
    local me = this(host)
    local d = {
        Treasure = TREASURE_DECK,
        Loot = LOOT_DECK,
        Monster = MONSTER_DECK,
    }
    local deckType = d[CardData[me.id].dChoice]
    local deck = getDiscard(host, deckType)
    if #deck == 0 then
        return
    end
    local last = deck[#deck]
    removeFromEverywhere(host, last.id)
    placeOnTop(host, deckType, last.id)
end
