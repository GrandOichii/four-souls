function CursedFatty_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function CursedFatty_trigger_check(host, me)
	return Common_LastRoll(host).value == 5
end

function CursedFatty_trigger(host)
	local roll = Common_LastRoll(host)
	Common_Discard(host, roll.ownerID, 1)
end
