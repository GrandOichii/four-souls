function TheCrowdfunder_cost(host, cardInfo)
    return Common_Pay(host, cardInfo.ownerID, 2) and Common_Roll(host, cardInfo.ownerID)
end

function TheCrowdfunder_fund(host)
    local roll = getLastRoll(host).value
    local owner = getTopOwner(host)
    if roll == 3 or roll == 4 then
        local choice = Common_ChoosePlayer(host, owner.id)
        dealDamage(host, PLAYER, owner.id, PLAYER, choice, 1)
        return
    end
    if roll == 5 or roll == 6 then
        local choice = Common_ChooseMonster(host, owner.id).id
        dealDamage(host, PLAYER, owner.id, MONSTER, choice, 1)
        return
    end
end