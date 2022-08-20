function MomsDeadHand_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 4)
end

function MomsDeadHand_death_cost(host, info)
	local ids = {}
	for _, player in ipairs(getPlayers(host)) do
		if player.id ~= info.ownerID then
			for _, card in ipairs(player.board) do
				if not card.isEternal then
					ids[#ids+1] = card.id
				end
			end
		end
	end
	if #ids == 0 then
		return false
	end
    local choice, _ = requestChoice(host, info.ownerID, 'Choose a non-eternal card', CARD, ids)
    pushTarget(host, choice, CARD)
	return true
    -- return Common_TargetNonEternalCard(host, info.ownerID)
end

function MomsDeadHand_death(host)
	local target = popTarget(host).id
	local exists = false
	for _, player in ipairs(getPlayers(host)) do
		for _, card in ipairs(player.board) do
			if card.id == target then
				exists = true
				break
			end
		end
		if exists then
			break
		end
	end
	if not exists then
		print('RETURNING')
		return
	end
	removeFromEverywhere(host, target)
	print('TARGET: '..target)
	moveToBoard(host, getCurrentPlayer(host).id, target)
end
