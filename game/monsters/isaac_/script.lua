function Isaac_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function Isaac_trigger_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function Isaac_trigger_cost(host, info)
    local player = getCurrentPlayer(host)
    return Common_TargetLivingPlayer(host, player.id)
end

function Isaac_trigger(host)
    local me = this(host)
    local target = popTarget(host)
    dealDamage(host, MONSTER, me.id, PLAYER, target.id, 1)
end
