function PsyHorf_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 1)
end

function PsyHorf_death(host)
	local p = getCurrentPlayer(host)
	for _, card in ipairs(p.board) do
		rechargeCard(host, card.id)
	end
end