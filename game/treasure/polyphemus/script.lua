function Polyphemus_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 2),
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function Polyphemus_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end