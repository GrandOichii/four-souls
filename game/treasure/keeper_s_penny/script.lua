function KeepersPenny_enter(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 9)
end

function KeepersPenny_effect(host)
    local owner = getTopOwner(host)
    local me = this(host)
    addCounters(host, me.id, 1)
    addCoins(host, owner.id, me.counters + 1)
    local choice = Common_ChoosePlayer(host, owner.id)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, owner.id, me.id)
end
