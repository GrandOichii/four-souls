function Dinner_enter(host)
    local owner = getOwner(host)
    incMaxLife(host, owner["id"], 1)
end

function Dinner_leave(host)
    local owner = getOwner(host)
    decMaxLife(host, owner["id"], 1)
end