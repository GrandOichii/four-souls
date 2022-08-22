--  TODO remake to use the stack

function Wishbone_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    gainTreasure(host, owner.id, 1)
end