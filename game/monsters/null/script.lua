function Null_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 6)
end

function Null_enter(host)
    local me = this(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == MONSTER and tgtID == me.id  then
                    CardData[me.id].counter = CardData[me.id].counter + 1
                    if CardData[me.id].counter % 2 == 0 then
                        return 0
                    end
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
		ipID = id,
        counter = 0
	}
end

function Null_leave(host)
    local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end

function Null_trigger(host)
    local me = this(host)
    CardData[me.id].counters = 0
end