function HolyDinga_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	addCoins(host, killer.id, roll.value)
end

function HolyDinga_trigger_check(host, me)
	return Common_LastRoll(host).value == 6
end

function HolyDinga_trigger(host)
	local roll = Common_LastRoll(host)
	healPlayer(host, roll.ownerID, 1)
end
