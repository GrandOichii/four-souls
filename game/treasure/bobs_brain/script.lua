function BobsBrain_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function BobsBrain_trigger_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function BobsBrain_trigger(host)
    local roll = getLastRoll(host).value
    local owner = getTopOwner(host)
    if roll == 1 or roll == 2 then
        local target = Common_ChooseMonster(host, owner.id)
        dealDamage(host, PLAYER, owner.id, MONSTER, target.id, 1)
        return
    end
    if roll == 3 or roll == 4 then
        local target = Common_ChoosePlayer(host, owner.id)
        print(target)
        dealDamage(host, PLAYER, owner.id, PLAYER, target, 1)
        return
    end
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
end