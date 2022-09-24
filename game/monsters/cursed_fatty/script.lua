function CursedFatty_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function CursedFatty_trigger_check(host, me)
	return Common_LastRoll(host, me).value == 5
end

function CursedFatty_trigger(host)
	local roll = CardData[this(host).id].lastRoll
	Common_Discard(host, roll.ownerID, 1, false)
end
