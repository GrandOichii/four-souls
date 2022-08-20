function War_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 8)
end

function War_enter(host, me)
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

function War_leave(host, me)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end

function War_trigger1_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function War_trigger1(host)
    local me = this(host)
    CardData[me.id].pInc = CardData[me.id].pInc + 1
end

function War_trigger2_check(host, me)
    return true
end

function War_trigger2(host)
    local me = this(host)
    CardData[me.id].pInc = 0
end
