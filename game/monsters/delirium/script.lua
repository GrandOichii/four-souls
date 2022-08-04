function Delirium_enter(host, me)
	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                return math.min(6, 1 + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function Delirium_leave(host, me)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Delirium_rewards(host, me, killer)
    gainTreasure(host, killer.id, 2)
end