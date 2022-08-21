function CursedHorf_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 3)
end

function CursedHorf_trigger_check(host, me)
	return Common_LastRoll(host, me).value == 2
end

function CursedHorf_trigger(host)
	local me = this(host)
	local roll = CardData[me.id].lastRoll
	dealDamage(host, MONSTER, me.id, PLAYER, roll.ownerID, 2)
end
