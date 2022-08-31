function TheLamb_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 3)
end

function TheLamb_death_cost(host, info)
	return Common_TargetOpponent(host, info.ownerID)
end

function TheLamb_death(host)
	local owner = getTopOwner(host)
	local target = popTarget(host).id
	local player = Common_PlayerWithID(host, target)
	if #player.souls == 0 then
		return
	end
	local choice, chosen = Common_ChooseSoul(host, target, target)
	if chosen then
		destroySoul(host, target, choice)
		addSoulCard(host, owner.id, choice)
	end
end
