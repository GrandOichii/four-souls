function RedGhost_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end

function RedGhost_trigger_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function RedGhost_trigger_cost(host, info)
    local player = getCurrentPlayer(host)
    return Common_TargetOpponent(host, player.id)
end

function RedGhost_trigger(host)
    --  TODO who deals the damage?
    local target = popTarget(host)
    local player = getCurrentPlayer(host)
    dealDamage(host, PLAYER, player.id, PLAYER, target.id, 1)
end
