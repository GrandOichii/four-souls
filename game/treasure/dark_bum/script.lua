function DarkBum_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function DarkBum_trigger(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        addCoins(host, owner.id, 3)
        return
    end
    if roll == 3 or roll == 4 then
        lootCards(host, owner.id, 1)
        return
    end
    if roll == 5 or roll == 6 then
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
        return
    end
end