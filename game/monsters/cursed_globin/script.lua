function CursedGlobin_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    lootCards(host, owner.id, roll.value)
end

function CursedGlobin_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 3
end

function CursedGlobin_trigger(host)
    for _, monster in pairs(getActiveMonsters(host)) do
        healMonster(host, monster.id, 2)
    end
end
