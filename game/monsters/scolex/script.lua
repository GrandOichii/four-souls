function Scolex_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 1)
end

function Scolex_trigger_check(host, me)
    return Common_MonsterDealtCombatDamage(host, me.id)
end

function Scolex_trigger(host)
    local me = this(host)
    local pid = CardData[me.id].damageEvent.targetID
    Common_Discard(host, pid, 1)
end
