function MomsEye_rewards(host, me, killer)
	lootCards(host, killer["id"], 1)
end

function MomsEye_death(host)
	local p = getCurrentPlayer(host)
	local choice = requestSimpleChoice(host, p.id, 'Look at a player\'s hand?', {'Yes', 'No'})
	if choice == 'No' then
		return
	end
	choice = Common_ChooseOpponent(host, p.id)
	local target = Common_PlayerWithID(host, choice)
	if #target.hand == 0 then
		return
	end
	requestCardsInHand(host, p.id, target.id, 'Looking at '..target.name..'\'s hand (click any card to continue)', 1)
end