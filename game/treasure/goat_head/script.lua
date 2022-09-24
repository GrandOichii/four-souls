function GoatHead_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function GoatHead_trigger(host)
    --  TODO rework due to rib of greed
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Discard a card?', {'Yes', 'No'})
    local counter = 0
    while choice == 'Yes' and #Common_PlayerWithID(host, owner.id).hand ~= 0 do
        counter = counter + 1
        Common_Discard(host, owner.id, 1, false)
        choice = requestSimpleChoice(host, owner.id, 'Discard another card?', {'Yes', 'No'})
    end
    lootCards(host, owner.id, counter)
end