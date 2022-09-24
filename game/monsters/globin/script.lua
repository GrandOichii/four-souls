function Globin_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end

function Globin_trigger_check(host, me)
    local m = Common_MonsterWithID(host, me.id)
    return m.health == 1
end

function Globin_trigger(host)
    --  TODO could not work all the time as worded
    --  TODO change damage source
    local me = this(host)
    local roll = popRollStack(host)
    if roll.value < 5 then
        dealDamage(host, MONSTER, me.id, MONSTER, me.id, 1)
        return
    end
    healMonster(host, me.id, 2)
end
