function CursedSoul_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CursedSoul_trigger1(host)
    local me = this(host)
    local owner = getTopOwner(host)
    addCounters(host, me.id, 1)
    me = this(host)
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
    if me.counters >= 6 then
        removeFromEverywhere(host, me)
        addSoulCard(host, owner.id, me.id)
    end
end

function CursedSoul_trigger2_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CursedSoul_trigger2_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo)
end

function CursedSoul_trigger2(host)
    local target = popTarget(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, target.id, me.id)
end
