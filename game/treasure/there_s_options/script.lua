function TheresOptions_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    incPurchaseAmount(host, owner.id)
end

function TheresOptions_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheresOptions_trigger(host)
    local owner = getTopOwner(host)
    incPurchaseAmount(host, owner.id)
end

function TheresOptions_cost(host, cardInfo)
    return getCurrentPlayer(host).id == cardInfo.ownerID
end

function TheresOptions_peek(host)
    local cards = topCardsOf(host, TREASURE_DECK, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local owner = getTopOwner(host)
    requestSimpleChoice(host, owner.id, 'Top card of the treasure deck is ${'..card.name..'}', {'Ok'})
end
