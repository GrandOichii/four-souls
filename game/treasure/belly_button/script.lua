function BellyButton_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function BellyButton_trigger1(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function BellyButton_trigger2_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function BellyButton_trigger2(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Recharge your character card?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    rechargeCharacterCard(host, owner.id)
end
