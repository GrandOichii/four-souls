function Wizoob_rewards(host, me, killer)
	lootCards(host, killer.id, 3)
end

function Wizoob_death_cost(host, info)
	return Common_TargetPlayer(host, info)
end

function Wizoob_death(host)
	local target = popTarget(host).id
	local player = Common_PlayerWithID(host, target)
	if #player.souls == 0 then
		return
	end
	local choice = Common_ChooseSoul(host, target, target)
	destroySoul(host, target, choice)
end