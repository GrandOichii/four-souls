function Brownie_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function Brownie_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local sub = 0
                if mid == me.id then
                    sub = CardData[me.id].sub
                end
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - sub)
            end
        }
    )
	CardData[me.id] = {
		irID = id,
        sub = 0
	}
end

function Brownie_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Brownie_trigger1(host)
    local me = this(host)
    CardData[me.id].sub = 0
end

function Brownie_trigger2_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function Brownie_trigger2(host)
    local me = this(host)
    CardData[me.id].sub = CardData[me.id].sub - 1
end
