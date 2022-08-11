function FireMind_enter(host, me, owner)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                local mult = 1
                if tgtType == PLAYER and tgtID == owner.id and roll == 1 then
					mult = 2
				end
                if srcType == PLAYER and srcID == owner.id and roll == 6 then
					mult = 2
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount * mult, roll)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function FireMind_leave(host, me, owner)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end
