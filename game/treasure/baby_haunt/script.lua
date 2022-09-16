function BabyHaunt_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function BabyHaunt_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end

