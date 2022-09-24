function Sloth_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 1)
end

function Sloth_death(host)
	local killerID = getLastKillerID(host)
	local p = Common_PlayerWithID(host, killerID)
	for _, card in ipairs(p.hand) do
        discardLoot(host, killerID, card.id, false)
	end
end