function Satan_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 2)
end

function Satan_trigger_check(host, me)
    return Common_AttackingPlayerRolled(host, me, 6)
end

function Satan_trigger_cost(host, cardInfo)
    local me = this(host)
    return Common_TargetLivingPlayer(host, CardData[me.id].pid)
end

function Satan_trigger(host)
    local target = popTarget(host).id
    killEntity(host, PLAYER, target)
end
