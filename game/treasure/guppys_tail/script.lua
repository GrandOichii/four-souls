--  TODO untested
function GuppysTail_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)

    incBeginningLoot(host, owner["id"])

end

function GuppysTail_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    decBeginningLoot(host, owner.id)

end

function GuppysTail_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function GuppysTail_trigger(host)
    local owner = getTopOwner(host)
    Common_Discard(host, owner.id, 1, false)
end