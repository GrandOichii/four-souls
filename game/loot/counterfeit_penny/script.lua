--  TODO? change this to a trigger

function CounterfeitPenny_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    incAdditionalCoins(host, owner.id)
end

function CounterfeitPenny_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)
    decAdditionalCoins(host, owner.id)
end