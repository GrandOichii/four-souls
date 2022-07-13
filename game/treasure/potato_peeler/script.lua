function PotatoPeeler_cost(host, cardInfo)
    return Common_Tap(host)
end

function PotatoPeeler_tap(host)
    local arr = {LOOT_DECK, TREASURE_DECK, MONSTER_DECK}
    for _, deckName in pairs(arr) do
        millDeck(host, deckName, 1)
    end
end