--  TODO move targeting to ChaosCard_cost

function ChaosCard_tap(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local flag = Common_SacrificeMe(host, me.id)
    if not flag then
        return
    end
    local choice1 = 'Kill a player'
    local choice2 = 'Kill a monster'
    local choice3 = 'Destroy an item'
    local choice4 = 'Destroy a soul'
    local choice = requestSimpleChoice(host, owner.id, 'Choose', {choice1, choice2, choice3, choice4})
    if choice == choice1 then
        local target = Common_ChoosePlayer(host, owner.id)
        killEntity(host, PLAYER, target)
        return
    end
    if choice == choice2 then
        local target = Common_ChooseMonster(host, owner.id)
        killEntity(host, MONSTER, target.id)
        return
    end
    if choice == choice3 then
        local target, flag = Common_ChooseNonEternalCard(host, owner.id)
        if not flag then
            return
        end
        destroyCard(host, target)
        return
    end
    if choice == choice4 then
        local choice, ownerID, chosen = Common_ChooseAnySoul(host, owner.id)
        if chosen then
            destroySoul(host, ownerID, choice)
        end
        return
    end
end
