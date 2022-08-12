

function ShinyRock_trigger_check(host, me)
    local activations = getActivations(host)
	local activation = activations[#activations]
    return activation.ownerID == me.ownerID
end

function ShinyRock_trigger(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 1)
end
