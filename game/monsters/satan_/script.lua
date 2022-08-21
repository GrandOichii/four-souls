function Satan_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 2)
end

function Satan_trigger_check(host, me)
    local lr = Common_LastRoll(host, me)
    local flag = lr.value == 6 and lr.isCombatRoll
    if not flag then
        return false
    end
    CardData[me.id] = {
        pid = lr.ownerID
    }
    return true
end

function Satan_trigger_cost(host, cardInfo)
    local me = this(host)
    return Common_TargetLivingPlayer(host, CardData[me.id].pid)
end

function Satan_trigger(host)
    local target = popTarget(host).id
    killEntity(host, PLAYER, target)
end
