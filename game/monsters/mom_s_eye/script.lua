function MomsEye_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 1)
end

function MomsEye_death_cost(host, info)
	return Common_TargetOpponent(host, info.ownerID)
end

function MomsEye_death(host)
	local target = popTarget(host)
	local player = Common_PlayerWithID(host, target.id)
	local aPlayer = getCurrentPlayer(host)
	local choice = requestSimpleChoice(host, aPlayer.id, 'Look at '..player.name..'\'s hand?', {'Yes', 'No'})
	if choice == 'No' then
		return
	end
	if #player.hand == 0 then
		return
	end
	requestCardsInHand(host, aPlayer.id, player.id, 'Looking at '..player.name..'\'s hand. Click any card to continue', 1)
end