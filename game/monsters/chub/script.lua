function Chub_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 7)
end

function Chub_trigger_check(host, me)
	return Common_AttackingPlayerRolled(host, me, 1)
end

function Chub_trigger(host)
	local me = this(host)
	healMonster(host, me.id, 2)
end
