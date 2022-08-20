function HaloOfFlies_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == PLAYER and tgtID == owner.id and roll == 1 and roll == 6 then
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

function HaloOfFlies_leave(host, me, owner)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end
