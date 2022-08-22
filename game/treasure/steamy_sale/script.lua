function SteamySale_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    decTreasureCost(host, owner.id, 5)
end

function SteamySale_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    incTreasureCost(host, owner.id, 5)
end