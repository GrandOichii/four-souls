function DaddyHaunt_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                local add = 0
                if tgtType == PLAYER and tgtID == owner.id then
                    add = 1
                end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount + add, roll)
            end
        }
    )
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            local add = 0
            if tgtType == PLAYER and tgtID == owner.id then
                add = 1
            end
            DamageFuncStack._et[DamageFuncStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount + add)
        end
    })
    CardData[me.id] = {
        cdID = id
    }
end

function DaddyHaunt_leave(host)
    local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].cdID)
    DamageFuncStack:remove('damage'..me.id)
end