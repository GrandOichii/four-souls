--  TODO untested

function CeremonialBlade_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CeremonialBlade_trigger1(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
    Common_Discard(host, owner.id, 1)
end

function CeremonialBlade_trigger2_check(host, me)
    return Common_OwnerKilledMonster(host, me.id)
end

function CeremonialBlade_trigger2(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function CeremonialBlade_tap(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end