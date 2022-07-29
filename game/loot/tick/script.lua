--  TODO untested

function Tick_trigger_check(host, me)
    return Common_OwnerKilledMonster(host, me.id) and Common_OncePerTurn(host, me.id)
end

function Tick_trigger(host)
    local owner = getTopOwner(host)
    healPlayer(host, owner.id, 1)
    incAttackCount(host, owner.id)
end