function HolyDinga_rewards_cost(host, info)
	return Common_Roll(host, info.ownerID)
end

function HolyDinga_rewards(host, me, killer)
	local roll = getLastRoll(host)
	addCoins(host, killer.id, roll.value)
end

function HolyDinga_trigger_check(host, me)
	return Common_LastRoll(host).value == 6
end

function HolyDinga_trigger(host)
	local roll = Common_LastRoll(host)
	healPlayer(host, roll.ownerID, 1)
end
