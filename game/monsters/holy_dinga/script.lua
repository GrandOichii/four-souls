function HolyDinga_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	addCoins(host, killer.id, roll.value)
end

function HolyDinga_trigger_check(host, me)
	return Common_LastRoll(host, me).value == 6
end

function HolyDinga_trigger(host)
	local me = this(host)
	local roll = CardData[me.id].lastRoll
	healPlayer(host, roll.ownerID, 1)
end
