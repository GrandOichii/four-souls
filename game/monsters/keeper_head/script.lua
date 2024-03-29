function KeeperHead_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	addCoins(host, killer.id, roll.value)
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
