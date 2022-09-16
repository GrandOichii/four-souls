function HolyPsyHorf_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end

function HolyPsyHorf_trigger(host)
    local player = getCurrentPlayer(host)
    lootCards(host, player.id, 1)
end
