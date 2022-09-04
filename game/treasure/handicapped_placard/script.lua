function HandicappedPlacard_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local diff = 0
                local o = Common_PlayerWithID(host, owner.id)
                local ms = Common_SoulCount(o)
                for _, player in ipairs(getPlayers(host)) do
                    local c = Common_SoulCount(player)
                    if c > diff then
                        diff = c
                    end
                end
                diff = diff - ms
                if diff < 0 then
                    diff = 0
                end
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - diff)
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function HandicappedPlacard_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end
