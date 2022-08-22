function Placenta_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == PLAYER and tgtID == owner.id and roll == 4 then
					return 0
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
        imlID = Common_IncMaxLife(host, owner.id, 1),
		ipID = id
	}
end

function Placenta_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    CombatDamageLayers:remove(CardData[me.id].ipID)
    Common_DecMaxLife(host, CardData[me.id].imlID, owner.id)
end