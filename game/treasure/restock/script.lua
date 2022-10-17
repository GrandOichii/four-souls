function Restock_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Restock_trigger(host)
    --  TODO? don't know whether the choice should be in cost
    local owner = getTopOwner(host)
    local shop = getShop(host)
    local activated = false
    for _, card in ipairs(shop) do
        local choice = requestSimpleChoice(host, owner.id, 'Discard ${'..card.key..'}?', {'Yes', 'No'})
        if choice == 'Yes' then
            activated = true
            removeFromEverywhere(host, card.id)
            discardMe(host, card.id)
        end
    end
    if activated then
        pushRefillShop(host)
    end
end
