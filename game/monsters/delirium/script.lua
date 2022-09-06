function Delirium_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    print(#MonsterRollLayers._et)
    for _, e in ipairs(MonsterRollLayers._et) do
        print('\t', e.id)
    end
    print(id)
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid ~= me.id then
                    add = 1
                end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function Delirium_leave(host)
    local me = this(host)
    for _, e in ipairs(MonsterRollLayers._et) do
        print('\t', e.id)
    end
    MonsterRollLayers:remove(CardData[me.id].irID)
    CardData[me.id] = nil
end

function Delirium_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 2)
end