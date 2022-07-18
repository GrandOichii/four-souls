function Abaddon_enter(host, me, owner)
    incAttack(host, owner.id, 5)
end

function Abaddon_leave(host, me, owner)
    decAttack(host, owner.id, 5)
end

function Abaddon_trigger_check(host, me)
    local owner = getOwner(host, me.id)
    local damageEvent = getDamageEvent(host)
    return damageEvent["targetType"] == PLAYER and damageEvent["sourceType"] == MONSTER and damageEvent["targetID"] == owner["id"]
end

function Abaddon_trigger(host)
    local owner = getTopOwner(host)
    killEntity(host, PLAYER, owner.id)
end