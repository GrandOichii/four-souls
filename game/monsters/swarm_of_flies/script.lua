function SwarmOfFlies_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 5)
end

function SwarmOfFlies_trigger_check(host, me)
    return Common_AttackingPlayerRolled(host, me, 5)
end

function SwarmOfFlies_trigger(host)
    local me = this(host)
    local pid = CardData[me.id].pid
    dealDamage(host, MONSTER, me.id, PLAYER, pid, 1)
end
