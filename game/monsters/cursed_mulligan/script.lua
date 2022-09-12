function CursedMulligan_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function CursedMulligan_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function CursedMulligan_trigger(host)
    expandActiveMonsters(host, 1)
end
