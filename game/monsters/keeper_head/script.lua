function KeeperHead_rewards(host, me, killer)
	--  TODO
	addCoins(host, killer.id, math.random(1, 6))
end

function KeeperHead_trigger_check(host, me)
	return Common_MonsterDealtCombatDamage(host, me.id)
end

function KeeperHead_trigger(host)
	local me = this(host)
	local de = CardData[me.id].damageEvent
	local p = Common_PlayerWithID(host, de.targetID)
	subCoins(host, p.id, math.min(2, p.coins))
end
