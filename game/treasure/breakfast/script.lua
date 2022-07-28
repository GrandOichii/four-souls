function Breakfast_enter(host, me, owner)
    CardData[me.id] = {
        imlID = Common_IncMaxLife(host, owner.id, 1)
    }
end

function Breakfast_leave(host, me, owner)
    Common_DecMaxLife(host, CardData[me.id].imlID, owner.id)
end