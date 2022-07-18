--  TODO untested
--  TODO fix name
function DaddysLongLegs_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function DaddysLongLegs_trigger(host)
    local owner = getTopOwner(host)
    local choice1 = 'Deal 1 damage to a player'
    local choice2 = 'Deal 1 damage to a monster'
    local choice = requestSimpleChoice(host, owner.id, 'Choose:', {choice1, choice2})
    if choice == choice1 then
        local target = Common_ChoosePlayer(host, owner.id)
        dealDamage(host, PLAYER, owner.id, PLAYER, target)
        return
    end
    local target = Common_ChooseMonster(host, owner.id)
    dealDamage(host, PLAYER, owner.id, MONSTER, target)
end
