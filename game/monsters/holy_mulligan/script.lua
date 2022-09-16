function HolyMulligan_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 3)
end

function HolyMulligan_death(host)
    local player = getCurrentPlayer(host)
    expandActiveMonsters(host, 2)
    Common_IncAttackCount(host, player.id, false)
end
