function SackHead_cost(host, cardInfo)
    return Common_Tap(host)
end

function SackHead_tap(host)
    local owner = getTopOwner(host)
    local choices = {LOOT_DECK, TREASURE_DECK, MONSTER_DECK}
    local deckType = requestSimpleChoice(host, owner["id"], "Look at the top of which deck?", choices)
    local card = topCardsOf(host, deckType, 1)[1]
    --  TODO? show card
    local choice = requestSimpleChoice(host, owner["id"], "Put "..card["name"].." on the bottom of the "..deckType.." deck?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    print('putting '..card["name"].." to bottom")
    putFromTopToBottom(host, deckType, 1)
    print('next card: '..topCardsOf(host, deckType, 1)[1]["name"])
    -- local n=0
    -- for k,v in pairs(tab) do
    --     n=n+1
    --     keyset[n]=k
    -- end

end