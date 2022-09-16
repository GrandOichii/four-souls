function Bony_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function Bony_enter(host)
    local me = this(host)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id then
                    add = CardData[me.id].inc
                end
                return MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid) + add
            end
        }
    )
    CardData[me.id] = {
        pwrID = id,
        inc = 0
    }
end

function Bony_leave(host)
    local me = this(host)
    MonsterPowerLayers:remove(CardData[me.id].pwrID)
end

function Bony_trigger1_check(host, me)
    return Common_AttackingPlayerRolled(host, me, 1)
end

function Bony_trigger1(host)
    local me = this(host)
    CardData[me.id].inc = CardData[me.id].inc + 2
end

function Bony_trigger2(host)
    local me = this(host)
    CardData[me.id].inc = 0
end
