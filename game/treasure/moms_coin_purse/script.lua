function MomsCoinPurse_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)

    incBeginningLoot(host, owner["id"])

end

function MomsCoinPurse_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    decBeginningLoot(host, owner["id"])

end