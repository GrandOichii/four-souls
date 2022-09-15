function Fistula_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 7)
end

function Fistula_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id then
                    add = CardData[me.id].add
                end
                return math.min(6, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) + add)
            end
        }
    )
	CardData[me.id] = {
		irID = id,
        add = 0
	}
end

function Fistula_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Fistula_trigger1(host)
    local me = this(host)
    CardData[me.id].sub = 0
end

function Fistula_trigger2_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function Fistula_trigger2(host)
    local me = this(host)
    CardData[me.id].add = CardData[me.id].add + 1
end
