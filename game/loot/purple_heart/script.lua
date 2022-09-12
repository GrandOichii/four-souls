function PurpleHeart_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function PurpleHeart_trigger(host)
    local cards = topCardsOf(host, TREASURE_DECK, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Put ${'..card.name..'} on the bottom of the loot deck?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    millDeck(host, LOOT_DECK, 1)
end
