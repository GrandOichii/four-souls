--  TODO untested

function TechX_ability1(host)
    local card = this(host)
    addCounters(host, card.id, 1)
end

function TechX_cost2(host, cardInfo)
    return Common_RemoveCounters(host, 3)
end

function TechX_ability2(host)
    local owner = getTopOwner(host)
    local choice1 = 'Kill a player'
    local choice2 = 'Kill a monster'
    local choice = requestSimpleChoice(host, owner.id, 'Choose:', {choice1, choice2})
    if choice == choice1 then
        local target = Common_ChoosePlayer(host, owner.id)
        killEntity(host, PLAYER, target.id)
        return
    end
    local target = Common_ChooseMonster(host, owner.id)
    killEntity(host, MONSTER, target.id)
end