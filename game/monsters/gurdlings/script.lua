function Gurdlings_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end

function Gurdlings_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local sub = 0
                if mid == me.id and getMonsterHealth(host, me.id) <= 2 then
                    sub = 1
                end
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - sub)
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function Gurdlings_leave(host)
local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end
