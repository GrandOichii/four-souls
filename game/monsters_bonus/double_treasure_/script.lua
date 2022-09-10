function DoubleTreasure_cost(host, info)
    local me = this(host)
    local players = getPlayers(host)
    local ids = {}
    for _, p in pairs(players) do
        if p.id ~= info.ownerID then
            ids[#ids+1] = p.id
        end
    end
    local choiceId, _ = requestChoice(host, info.ownerID, 'Choose an opponent for ${'..me.name..'}', PLAYER, ids)
    print('PUSHED')
    pushTarget(host, choiceId, PLAYER)
    return true
end

function DoubleTreasure_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    gainTreasure(host, owner.id, 1)
    gainTreasure(host, target.id, 1)
    Common_BonusMonsterTail(host, me.id)
end
