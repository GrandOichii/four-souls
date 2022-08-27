function LazarusRags_trigger_check(host, me)
    return Common_PostDeathOwnerDied(host, me.id)
end

function LazarusRags_trigger(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end