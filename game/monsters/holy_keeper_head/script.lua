function HolyKeeperHead_rewards(host, me, killer)
	--  TODO
	addCoins(host, killer.id, math.random(1, 6))
end

function HolyKeeperHead_trigger_check(host, me)
	return Common_LastRoll(host).value == 4
end

function HolyKeeperHead_trigger(host)
	local roll = Common_LastRoll(host)
	addCoins(host, roll.ownerID, 2)
end
