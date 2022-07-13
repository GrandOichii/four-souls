--  TODO? change this to a trigger - will have to rework coin gaining system to a system, similar to the one damage uses :(

function CounterfeitPenny_enter(host, me, owner)
    incAdditionalCoins(host, owner["id"])
end

function CounterfeitPenny_leave(host, me, owner)
    decAdditionalCoins(host, owner["id"])
end