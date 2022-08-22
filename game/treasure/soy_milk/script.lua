function SoyMilk_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function SoyMilk_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)

end

function SoyMilk_trigger_check(host, me)
    return Common_OwnerDealtCombatDamage(host, me.id)
end

function SoyMilk_trigger(host)
    local owner = getTopOwner(host)
    local dEvent = getDamageEvent(host)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        if monster.id ~= dEvent.targetID then
            dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
        end
    end
end
