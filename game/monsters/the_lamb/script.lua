function TheLamb_rewards(host, me, killer)
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
	local choice = Common_ChooseSoul(host, target, target)
	destroySoul(host, target, choice)
	addSoulCard(host, owner.id, choice)
end
