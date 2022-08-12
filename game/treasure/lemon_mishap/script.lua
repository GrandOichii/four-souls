function LemonMishap_enter(host, me, owner)
    CardData[me.id] = {
        activated = false
    }
end

function LemonMishap_cost(host)
    return Common_Tap(host)
end

function LemonMishap_tap(host)
    local me = this(host)
    CardData[me.id].activated = true
end

function LemonMishap_trigger1_check(host, me)
    if not CardData[me.id].activated then
        return false
    end
    local lr = Common_LastRoll(host).value
    return lr.isCombatRoll and (lr == 1 or lr == 2)
end

function LemonMishap_trigger1(host)
    local owner = getTopOwner(host)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
    end
end

function LemonMishap_trigger2_check(host, me)
    return true
end

function LemonMishap_trigger2(host)
    local me = this(host)
    CardData[me.id].activated = false
end
