function FlamingHopper_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 4)
end

function FlamingHopper_enter(host)
    local me = this(host)

    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == MONSTER and tgtID == me.id and roll == 6 then
					return 0
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function FlamingHopper_leave(host)
    local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end
