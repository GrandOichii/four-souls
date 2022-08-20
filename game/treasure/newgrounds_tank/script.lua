--  TODO untested

function NewgroundsTank_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 1),
        ihID = Common_IncMaxLife(host, owner.id, 1),
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function NewgroundsTank_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
    Common_DecMaxLife(host, CardData[me.id].ihID, owner.id)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end