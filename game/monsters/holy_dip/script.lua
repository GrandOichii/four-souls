function HolyDip_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 1)
end

function HolyDip_trigger_check(host, me)
	return Common_LastRoll(host, me).value == 1
end

function HolyDip_trigger(host)
	local roll = CardData[this(host).id].lastRoll
	addCoins(host, roll.ownerID, 1)
end
