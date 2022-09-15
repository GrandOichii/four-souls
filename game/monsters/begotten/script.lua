function Begotten_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end

function Begotten_enter(host)
    local me = this(host)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id and getMonsterHealth(host, mid) == 1 then
                    add = 1
                end
                return add + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
    CardData[me.id] = {
        pwrID = id
    }
end

function Begotten_leave(host)
    local me = this(host)
    MonsterPowerLayers:remove(CardData[me.id].pwrID)
end
