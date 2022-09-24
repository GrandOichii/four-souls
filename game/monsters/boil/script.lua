function Boil_trigger_check(host, me)
    return Common_MonsterDealtCombatDamage(host, me.id)
end

function Boil_trigger(host)
    local me = this(host)
    healMonster(host, me.id, 1)
end

function Boil_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 3)
end