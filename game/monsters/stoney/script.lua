--  TODO

function Stoney_enter(host, me)
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

function Stoney_leave(host, me)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Stoney_rewards(host, me, killer)
	lootCards(host, killer["id"], 1)
end