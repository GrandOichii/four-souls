function Abaddon_enter(host, me, owner)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 3)
    }
end

function Abaddon_leave(host, me, owner)
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