function HolySquirt_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function HolySquirt_trigger_check(host, me)
	return Common_LastRoll(host).value == 5
end

function HolySquirt_trigger(host)
	local ownerID = Common_LastRoll(host).ownerID
	lootCards(host, ownerID, 1)
end
