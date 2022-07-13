function Dinner_enter(host, me, owner)
    incMaxLife(host, owner["id"], 1)
end

function Dinner_leave(host, me, owner)
    decMaxLife(host, owner["id"], 1)
end