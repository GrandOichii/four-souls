function Bomb_cost(host, cardInfo)

    local owner = Common_PlayerWithID(host, cardInfo.ownerID)
    local choice1 = 'Player'
    local choice2 = 'Monster'
    local choice = requestSimpleChoice(host, owner.id, 'Deal damage to who?', {choice1, choice2})
    if choice == choice1 then
        local target = Common_ChoosePlayer(host, owner.id)
        pushTarget(host, target, PLAYER)
        return true
    end
    local monster = Common_ChooseMonster(host, owner.id)
    pushTarget(host, monster.id, MONSTER)
    return true
end

function Bomb_use(host)
    discardMe(host, this(host).id)

    local target = popTarget(host)
    dealDamage(host, PLAYER, getTopOwner(host).id, target.type, target.id, 1)

end