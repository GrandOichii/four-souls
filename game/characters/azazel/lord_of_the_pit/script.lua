function LordOfThePit_cost(host, cardInfo)
    return Common_IsCombat(host)
end

function LordOfThePit_tap(host)
    cancelCurrentAttack(host)
    local player = getCurrentPlayer(host)
    Common_IncAttackCount(host, player.id, false)
end