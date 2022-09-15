function CurseOfEmpathy_trigger_check(host, me)
    if Common_CurseDeathCheck(host, me) then
        pushToStack(host, 'Common_CurseDeathEffect', me.ownerID, me.id, 'trigger')
        return false
    end
    return Common_LastDeath(host).type == MONSTER
end

function CurseOfEmpathy_trigger(host)
    local owner = getTopOwner(host)
    Common_Discard(host, owner.id, 1)
    local amount = math.min(1, owner.coins)
    subCoins(host, owner.id, amount)
end
