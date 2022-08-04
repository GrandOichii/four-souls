function DeathsHead_rewards(host, me, killer)
    lootCards(host, killer.id, 1)
end

function DeathsHead_enter(host, me)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                return 1 + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function DeathsHead_leave(host, me)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end
