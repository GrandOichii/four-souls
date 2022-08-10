function DeadEye_trigger_check(host, me)
    return Common_OwnerDealtDamage(host, me.id)
end

function DeadEye_trigger(host)
    local _ = getDamageEvent(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_TempIncAttack(host, me.id, owner.id, 1)
end