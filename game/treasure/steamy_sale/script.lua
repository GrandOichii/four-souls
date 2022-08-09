function SteamySale_enter(host, me, owner)
    decTreasureCost(host, owner.id, 5)
end

function SteamySale_leave(host, me, owner)
    incTreasureCost(host, owner.id, 5)
end