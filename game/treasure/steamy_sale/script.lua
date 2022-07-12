function SteamySale_enter(host)
    local owner = getOwner(host) -- works for some reason
    decTreasureCost(host, owner["id"], 5)
end

function SteamySale_leave(host)
    local owner = getOwner(host) -- works for some reason
    incTreasureCost(host, owner["id"], 5)
end