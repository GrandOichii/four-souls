--  TODO untested

function BlackPills_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function BlackPills_use(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
    if roll == 1 or roll == 2 then
        local target = Common_ChoosePlayer(host, owner.id)
        dealDamage(host, PLAYER, owner.id, PLAYER, target, 1)
        return
    end
    if roll == 3 or roll == 4 then
        local target = Common_ChooseMonster(host, owner.id)
        dealDamage(host, PLAYER, owner.id, MONSTER, target.id)
        return
    end
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
end