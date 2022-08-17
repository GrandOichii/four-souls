function HolyKeeperHead_rewards_cost(host, info)
	return Common_Roll(host, info.ownerID)
end

function HolyKeeperHead_rewards(host, me, killer)
	local roll = getLastRoll(host)
	addCoins(host, killer.id, roll.value)
end

function HolyKeeperHead_trigger_check(host, me)
	return Common_LastRoll(host).value == 4
end

function HolyKeeperHead_trigger(host)
	local roll = Common_LastRoll(host)
	addCoins(host, roll.ownerID, 2)
end
