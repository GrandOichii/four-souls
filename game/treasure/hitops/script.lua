--  TODO untested

function Hitops_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == PLAYER and (tgtID == owner.id or CardData[me.id].activated) and (roll == 4 or roll == 5) then
					return 0
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
		ipID = id,
        activated = false
	}
end

function Hitops_leave(host)
    local me = this(host)

    CombatDamageLayers:remove(CardData[me.id].ipID)
end

function Hitops_trigger_check(host, me)
    return not Common_OwnersTurn(host, me.id)
end

function Hitops_trigger_cost(host, info)
    return requestSimpleChoice(host, info.ownerID, 'Offer '..getCurrentPlayer(host).name..' a high-five?', {'Yes', 'No'}) == 'Yes'
end

function Hitops_trigger(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local player = getCurrentPlayer(host)
    CardData[me.id].activated = requestSimpleChoice(host, player.id, 'High-five '..owner.name..'?', {'Yes', 'No'}) == 'Yes'
end
