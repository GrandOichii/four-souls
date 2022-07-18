--  TODO untested

function TheRelic_trigger_check(host, me)
    return Common_LastRoll(host).value == 1
end

function TheRelic_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end
