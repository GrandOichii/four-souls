function MomsRing_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 1)
    }
end

function MomsRing_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end