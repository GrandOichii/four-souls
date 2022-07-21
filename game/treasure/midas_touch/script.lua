function MidasTouch_trigger_check(host, cardInfo)
    local death = Common_LastDeath(host)
    return death.type == MONSTER
end

function MidasTouch_trigger(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 3)
end