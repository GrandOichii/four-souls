--  TODO untested

function TheHighPriestess_cost(host, cardInfo)
    local choice1 = 'Player'
    local choice2 = 'Monster'
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Deal damage to who?', {choice1, choice2})
    if choice == choice1 then
        Common_TargetPlayer(host, cardInfo.ownerID)
    else
        Common_TargetMonster(host, cardInfo.ownerID)
    end
    return Common_Roll(host, cardInfo.ownerID)
end

function TheHighPriestess_use(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
    dealDamage(host, PLAYER, owner.id, target.type, target.id, roll)
end
