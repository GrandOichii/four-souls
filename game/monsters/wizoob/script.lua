function Wizoob_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 3)
end

function Wizoob_death_cost(host, info)
	return Common_TargetPlayer(host, info.ownerID)
end

function Wizoob_death(host)
	local target = popTarget(host).id
	print('TARGET ID: '..target.id)
	local player = Common_PlayerWithID(host, target)
	if #player.souls == 0 then
		return
	end
	local choice = Common_ChooseSoul(host, target, target)
	destroySoul(host, target, choice)
end