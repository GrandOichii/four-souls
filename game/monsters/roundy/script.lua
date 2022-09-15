function Roundy_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 6)
end

function Roundy_trigger_check(host, me)
    return Common_MonsterDealtCombatDamage(host, me.id)
end

function Roundy_trigger(host)
    local me = this(host)
    for _, card in ipairs(getActiveMonsters(host)) do
        if me.id ~= card.id then
            dealDamage(host, MONSTER, me.id, MONSTER, card.id, 1)
        end
    end
end
