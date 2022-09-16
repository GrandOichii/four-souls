function BabyPlum_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    addCoins(host, owner.id, roll.value)
end

function BabyPlum_trigger_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function BabyPlum_trigger(host)
    --  TODO who deals the damage
    local me = this(host)
    local roll = popRollStack(host)
    local player = getCurrentPlayer(host)
    local players = getPlayers(host)
    local pI = 0
    for i, p in ipairs(players) do
        if p.id == player.id then
            pI = i
            break
        end
    end
    if roll.value == 3 or roll.value == 4 then
        pI = pI + 1
        if pI == #players + 1 then
            pI = 1
        end
    end
    if roll.value == 5 or roll.value == 6 then
        pI = pI - 1
        if pI == 0 then
            pI = #players
        end
    end
    dealDamage(host, MONSTER, me.id, PLAYER, players[pI].id, 1)
end
