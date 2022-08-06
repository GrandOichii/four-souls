function CursedHorf_rewards(host, me, killer)
	addCoins(host, killer["id"], 3)
end

function CursedHorf_trigger_check(host, me)
	return Common_LastRoll(host).value == 2
end

function CursedHorf_trigger(host)
	local me = this(host)
	local roll = Common_LastRoll(host)
	dealDamage(host, MONSTER, me.id, PLAYER, roll.ownerID, 2)
end
