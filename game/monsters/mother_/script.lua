function Mother_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function Mother_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id then
                    add = CardData[me.id].inc
                end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id,
        inc = 0
	}
end

function Mother_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Mother_trigger1_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function Mother_trigger1(host)
    local me = this(host)
    CardData[me.id].inc = CardData[me.id].inc + 1
end

function Mother_trigger2(host)
    local me = this(host)
    CardData[me.id].inc = 0
end