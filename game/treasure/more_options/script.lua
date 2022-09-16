function MoreOptions_enter(host)
    local owner = getTopOwner(host)
    incPurchaseAmount(host, owner.id)
end

function MoreOptions_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function MoreOptions_trigger(host)
    --  TODO? cancel fizzling if not enough cards
    --  TODO order might not be correct
    local owner = getTopOwner(host)
    incPurchaseAmount(host, owner.id)
    local choice = requestSimpleChoice(host, owner.id, 'Look at the top 2 cards of the treasure deck?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local cards = topCardsOf(host, TREASURE_DECK, 2)
    if #cards ~= 2 then
        return
    end
    local choices = {}
    for _, card in ipairs(cards) do
        choices[#choices+1] = '${'..card.name..'}'
    end
    requestSimpleChoice(host, owner.id, 'Top cards of the treasure deck (click any to continue)', choices)
end
