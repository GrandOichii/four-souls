function DiogenesPot_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == PLAYER and tgtID == owner.id and roll == 2 and roll == 3 then
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

function DiogenesPot_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    CombatDamageLayers:remove(CardData[me.id].ipID)

end

function DiogenesPot_trigger_check(host, me)
    local roll = Common_LastRoll(host, me)
    return roll.ownerID == me.ownerID and roll.isCombatRoll and roll.value == 1
end

function DiogenesPot_trigger(host)
    local owner = getTopOwner(host)
    killEntity(host, PLAYER, owner.id)
end
