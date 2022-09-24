function Brain_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function Brain_trigger_check(host, me)
    return Common_AttackingPlayerRolled(host, me, 6)
end

function Brain_trigger(host)
    local me = this(host)
    local pid = CardData[me.id].pid
    dealDamage(host, MONSTER, me.id, PLAYER, pid, 1)
end
