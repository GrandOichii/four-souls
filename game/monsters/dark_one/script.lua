function DarkOne_rewards(host, me, killer)
    gainTreasure(host, killer.id, 1)
	addSouls(host, killer.id, 1)
end

function DarkOne_enter(host, me)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id then
                    add = CardData[me.id].pInc
                end
                return add + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
    CardData[me.id] = {
        pInc = 0,
        ipID = id
    }
end

function DarkOne_leave(host, me)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end

function DarkOne_trigger1_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function DarkOne_trigger1(host)
    local me = this(host)
    CardData[me.id].pInc = CardData[me.id].pInc + 1
end

function DarkOne_trigger2_check(host, me)
    return true
end

function DarkOne_trigger2(host)
    local me = this(host)
    CardData[me.id].pInc = 0
end
