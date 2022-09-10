function Ambush_effect(host)
	local me = this(host)
    local player = getCurrentPlayer(host)
	addAttackOpportunity(host, player.id, true, {-1})
	addAttackOpportunity(host, player.id, true, {-1})
	Common_BonusMonsterTail(host, me.id)
end
