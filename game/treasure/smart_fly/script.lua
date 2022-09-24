function SmartFly_cost(host, info)
    local me = this(host)
    local choice = requestSimpleChoice(host, info.ownerID, 'Choose a deck', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    CardData[me.id].dChoice = choice
    return true
end

function SmartFly_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local deckType = CardData[me.id].dChoice
    local cards = topCardsOf(host, deckType, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local choice = requestSimpleChoice(host, owner.id, 'Discard ${'..card.key..'}?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    putFromTopToBottom(host, deckType, 1)
end

function SmartFly_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function SmartFly_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
