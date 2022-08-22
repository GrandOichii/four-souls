--  TODO untested

function DryBaby_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            if tgtType == PLAYER and tgtID == owner.id then
                amount = 1
            end
            DamageFuncStack._et[DamageFuncStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount)
        end
    })
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == PLAYER and tgtID == owner.id then
					amount = 1
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function DryBaby_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    DamageFuncStack:remove('damage'..me.id)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end
