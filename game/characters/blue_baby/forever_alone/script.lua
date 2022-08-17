local ForeverAlone_choice1 = "Steal 1c from another player."
local ForeverAlone_choice2 = "Loot at the top card of a deck."
local ForeverAlone_choice3 = "Discard a loot card, then loot 1."

function ForeverAlone_cost(host, cardInfo)
    local tapped = Common_Tap(host)
    if not tapped then
        return false
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, "Choose:", {ForeverAlone_choice1, ForeverAlone_choice2, ForeverAlone_choice3})
    local me = this(host)
    CardData[me.id] = {
        choice = choice
    }
    if choice == ForeverAlone_choice1 then
        return Common_TargetOpponent(host, cardInfo)
    end
    if choice == ForeverAlone_choice2 then
        local dc = requestSimpleChoice(host, cardInfo.ownerID, 'Look at the top of which deck?', {LOOT_DECK, TREASURE_DECK, MONSTER_DECK})
        CardData[me.id].dc = dc
    end
    return true
end

function ForeverAlone_tap(host)
    --  TODO
    local me = this(host)
    local owner = getTopOwner(host)
    local choice = CardData[me.id].choice
    if choice == ForeverAlone_choice1 then
        local target = popTarget(host)
        local player = Common_PlayerWithID(host, target.id)
        local amount = math.min(player.coins, 1)
        subCoins(host, player.id, amount)
        addCoins(host, owner.id, amount)
        return
    end
    if choice == ForeverAlone_choice2 then
        local dc = CardData[me.id].dc
        local cards = topCardsOf(host, dc, 1)
        if #cards == 0 then
            return false
        end
        local card = cards[1]
        requestSimpleChoice(host, owner.id, 'Top card of '..dc..' deck is: ${'..card.name..'}', {'Ok'})
        return
    end
    if choice == ForeverAlone_choice3 then
        --  TODO? fizzle if can't discard
        Common_Discard(host, owner.id, 1)
        lootCards(host, owner.id, 1)
    end

end

function ForeverAlone_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function ForeverAlone_trigger(host)
    local _ = getDamageEvent(host)
    local card = this(host)
    rechargeCard(host, card.id)
end