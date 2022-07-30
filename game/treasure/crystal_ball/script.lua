function CrystalBall_enter(host, me, owner)
    CardData[me.id] = {
        activated = false,
        roll = -1
    }
end

function CrystalBall_cost(host, cardInfo)
    return Common_Tap(host)
end

function CrystalBall_tap(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Predict the next dice roll', {'1', '2', '3', '4', '5', '6'})
    local me = this(host)
    CardData[me.id] = {
        activated = true,
        roll = tonumber(choice)
    }
end

function CrystalBall_trigger_check(host, me)
    local roll = Common_LastRoll(host)
    if not CardData[me.id].activated then
        return false
    end
    CardData[me.id].activated = false
    return CardData[me.id].roll == roll.value
end

function CrystalBall_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end
