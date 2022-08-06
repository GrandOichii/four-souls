function HolyDinga_rewards(host, me, killer)
	--  TODO
	addCoins(host, killer.id, math.random(1, 6))
end

function HolyDinga_trigger_check(host, me)
	return Common_LastRoll(host).value == 6
end

function HolyDinga_trigger(host)
	local roll = Common_LastRoll(host)
	healPlayer(host, roll.ownerID, 1)
end
