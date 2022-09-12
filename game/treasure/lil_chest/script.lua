function LilChest_trigger_check(host, me)
    return Common_OwnerKilledMonster(host, me.id)
end

function LilChest_trigger(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value < 3 then
        addCoins(host, owner.id, 2)
        return
    end
    if roll.value > 4 then
        for _, card in ipairs(owner.board) do
            rechargeCard(host, card.id)
        end
        return
    end
    lootCards(host, owner.id, 2)
    owner = Common_PlayerWithID(host, owner.id)
    local message = "Choose a card to place on top of the loot deck"
    if #owner.hand == 0 then
        return
    end
    local result = requestCardsInHand(host, owner.id, owner.id, message, 1)[1]
    removeFromEverywhere(host, result)
    placeOnTop(host, LOOT_DECK, result)
end
