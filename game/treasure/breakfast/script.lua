function Breakfast_enter(host, me, owner)
    Common_IncMaxLife(host, me.id, owner.id, 1)
end

function Breakfast_leave(host, me, owner)
    Common_DecMaxLife(host, me.id, owner.id)
end