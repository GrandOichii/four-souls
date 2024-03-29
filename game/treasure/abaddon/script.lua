function Abaddon_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 3)
    }
end

function Abaddon_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end

function Abaddon_trigger_check(host, me)
    local owner = getOwner(host, me.id)
    local damageEvent = getTopDamageEvent(host)
    return damageEvent["targetType"] == PLAYER and damageEvent["sourceType"] == MONSTER and damageEvent["targetID"] == owner["id"]
end

function Abaddon_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    killEntity(host, PLAYER, owner.id)
end