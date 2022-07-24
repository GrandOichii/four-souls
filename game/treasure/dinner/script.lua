function Dinner_enter(host, me, owner)
    Common_IncMaxLife(me.id, owner.id, 1)
end

function Dinner_leave(host, me, owner)
    Common_DecMaxLife(me.id)
end