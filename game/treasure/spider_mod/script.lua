function SpiderMod_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 5
end

function SpiderMod_trigger_cost(host, info)
    local ownerID = info.ownerID
    local me = this(host)
    local choice = requestSimpleChoice(host, ownerID, 'Use effect of ${'..me.key..'}', {'Yes', 'No'})
    if choice == 'No' then
        return false
    end
    return Common_TargetNonAttackedMonster(host, ownerID)
end

function SpiderMod_trigger(host)
    local target = popTarget(host)
    local piles = getMonsterPiles(host)
    for pileI, pile in pairs(piles) do
        if #pile ~= 0 then
            local card = pile[#pile]
            if card.id == target.id then
                removeFromEverywhere(host, card.id)
                discardMe(host, card.id)
                local cards = topCardsOf(host, MONSTER_DECK, 1)
                if #cards == 0 then
                    return
                end
                card = cards[1]
                removeFromEverywhere(host, card.id)
                placeInMonsterPile(host, pileI, card.id)
                return
            end
        end
    end
end
