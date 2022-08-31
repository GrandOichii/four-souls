function Gluttony_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 1)
end

function Gluttony_trigger_check(host, me)
	return false
	--  TODO unfinished
end

function Gluttony_trigger(host)

end
