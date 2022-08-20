function BabyHaunt_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function BabyHaunt_leave(host, me, owner)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end

function BabyHaunt_trigger_check(host, me)
    return Common_OwnerDied(host, me.ownerID)
end

function BabyHaunt_trigger_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo.ownerID)
end

function BabyHaunt_trigger(host)
    local me = this(host)
    local target = popTarget(host)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, target.id, me.id)
end
