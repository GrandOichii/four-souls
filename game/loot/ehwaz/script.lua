function Ehwaz_use(host)
    local me = this(host)
    local mPiles = getMonsterPiles(host)
    for i, pile in ipairs(mPiles) do
        if #pile ~= 0 then
            local card = pile[#pile]
            removeFromEverywhere(host, card.id)
            discardMe(host, card.id)
        end
        local cards = topCardsOf(host, MONSTER_DECK, 1)
        if #cards == 0 then
            return
        end
        local card = cards[1]
        placeInMonsterPile(host, i-1, card.id)
        removeFromEverywhere(host, card.id)
    end
    discardMe(host, me.id)
end
