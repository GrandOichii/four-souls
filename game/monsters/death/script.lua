function Death_rewards(host, me, killer)
    gainTreasure(host, killer.id, 1)
end

function Death_death(host)
    local p = getCurrentPlayer(host)
    local target = Common_ChoosePlayer(host, p.id)
    killEntity(host, PLAYER, target)
end