function MonstroII_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function MonstroII_trigger_check(host, me)
    return Common_MonsterDealtCombatDamage(host, me.id)
end

function MonstroII_trigger(host)
    local me = this(host)
    local player = getCurrentPlayer(host)
    local players = getPlayers(host)
    local roll = popRollStack(host)
    local pI = 0
    for i, p in ipairs(players) do
        if p.id == player.id then
            pI = i
            break
        end
    end
    if roll.value > 3 then
        pI = pI + 1
        if pI == #players + 1 then
            pI = 1
        end
    else
        pI = pI - 1
        if pI == 0 then
            pI = #players
        end
    end
    dealDamage(host, MONSTER, me.id, PLAYER, players[pI].id, 1)
end