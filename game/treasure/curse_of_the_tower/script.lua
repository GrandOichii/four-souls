--  TODO untested

function CurseOfTheTower_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function CurseOfTheTower_trigger_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function CurseOfTheTower_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
    if roll < 4 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            if player.id ~= owner.id then
                dealDamage(host, PLAYER, owner.id, PLAYER, player.id, 1)
            end
        end
        return
    end
    local monster = Common_ChooseMonster(host, owner.id)
    dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
end
