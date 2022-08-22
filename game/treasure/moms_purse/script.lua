function MomsPurse_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)

    incBeginningLoot(host, owner["id"])

end

function MomsPurse_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    decBeginningLoot(host, owner["id"])

end