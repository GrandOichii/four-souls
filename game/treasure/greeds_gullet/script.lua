function GreedsGullet_trigger_check(host, cardInfo)
    return Common_OwnerDied(host, cardInfo.ownerID)
end

function GreedsGullet_trigger(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 8)
end