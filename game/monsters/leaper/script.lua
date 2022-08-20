function Leaper_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 5)
end

function Leaper_enter(host, me)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
				local mult = 1
                if srcType == MONSTER and srcID == me.id and roll == 1 then
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

function Leaper_leave(host, me)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end