--  TODO untested

function Lard_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        imlID = Common_IncMaxLife(host, owner.id, 2)
    }
end

function Lard_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecMaxLife(host, CardData[me.id].imlID, owner.id)
end

function Lard_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function Lard_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    Common_Discard(host, owner.id, 1, false)
end