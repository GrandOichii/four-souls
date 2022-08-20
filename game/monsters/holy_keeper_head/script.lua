function HolyKeeperHead_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	addCoins(host, killer.id, roll.value)
end

function HolyKeeperHead_trigger_check(host, me)
	return Common_LastRoll(host).value == 4
end

function HolyKeeperHead_trigger(host)
	local roll = Common_LastRoll(host)
	addCoins(host, roll.ownerID, 2)
end
