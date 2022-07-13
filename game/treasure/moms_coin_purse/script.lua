function MomsCoinPurse_enter(host, me, owner)

    incBeginningLoot(host, owner["id"])

end

function MomsCoinPurse_leave(host, me, owner)

    decBeginningLoot(host, owner["id"])

end