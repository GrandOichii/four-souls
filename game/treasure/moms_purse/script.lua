function MomsPurse_enter(host, me, owner)

    incBeginningLoot(host, owner["id"])

end

function MomsPurse_leave(host, me, owner)

    decBeginningLoot(host, owner["id"])

end