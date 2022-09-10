function CurseOfTheHunted_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)

    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if getCurrentPlayer(host).id == owner.id then
                    add = 1
                end
                return add + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function CurseOfTheHunted_leave(host)
    local me = this(host)

    MonsterPowerLayers:remove(CardData[me.id].ipID)
end
