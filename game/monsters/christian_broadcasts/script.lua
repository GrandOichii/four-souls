function ChristianBroadcasts_death_cost(host, info)
    return Common_TargetMonster(host, info.ownerID) and Common_TargetPlayer(host, info.ownerID)
end

function ChristianBroadcasts_death_effect(host)
    local pTarget = popTarget(host)
    local mTarget = popTarget(host)
    killEntity(host, pTarget.type, pTarget.id)
    killEntity(host, mTarget.type, mTarget.id)
    expandActiveMonsters(host, 2)
end

function ChristianBroadcasts_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end
