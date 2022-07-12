function Breakfast_enter(host)
    local owner = getOwner(host)
    incMaxLife(host, owner["id"], 1)
end

function Breakfast_leave(host)
    local owner = getOwner(host)
    decMaxLife(host, owner["id"], 1)
end