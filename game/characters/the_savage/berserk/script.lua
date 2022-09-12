function Berserk_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Berserk_trigger1(host)
    local owner = getTopOwner(host)
    Common_IncAttackCount(host, owner.id, false)
end

function Berserk_trigger2_check(host, me)
    return Common_OwnerKilledMonster(host, me.id)
end

function Berserk_trigger2(host)
    local owner = getTopOwner(host)
    local me = this(host)
    Common_TempIncMaxLife(host, me.id, owner.id, 1)
end