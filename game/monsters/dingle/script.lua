function Dingle_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 1)
end

function Dingle_trigger_check(host, me)
    local damageEvent = getTopDamageEvent(host)
    return damageEvent.targetType == MONSTER and damageEvent.targetID == me.id and damageEvent.sourceType == PLAYER and damageEvent.sourceID == getCurrentPlayer(host).id
end

function Dingle_trigger(host)
    --  TODO remove damage source
    local me = this(host)
    local cPlayer = getCurrentPlayer(host)
    local roll = popRollStack(host)
    if roll.value < 3 then
        dealDamage(host, NO_SOURCE, NO_SOURCE_ID, PLAYER, cPlayer.id, 1)
        return
    end
    if roll.value > 4 then
        dealDamage(host, NO_SOURCE, NO_SOURCE_ID, MONSTER, me.id, 1)
        return
    end
    for _, player in ipairs(getPlayers(host)) do
        dealDamage(host, NO_SOURCE, NO_SOURCE_ID, PLAYER, player.id, 1)
    end
end
