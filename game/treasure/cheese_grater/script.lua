function CheeseGrater_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 6
end

function CheeseGrater_trigger(host)
    local choices = {LOOT_DECK, TREASURE_DECK, MONSTER_DECK}
    local owner = getTopOwner(host)
    local deckType = requestSimpleChoice(host, owner.id, 'Loot at the top of which deck?', choices)
    local cards = topCardsOf(host, deckType, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local choice = requestSimpleChoice(host, owner.id, 'Put ${'..card.name..'} into discard?', {'Yes', 'No'})
    if choice == 'No' then return end
    millDeck(host, deckType, 1)
end