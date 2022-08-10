function Pin_rewards(host, me, killer)
	addCoins(host, killer.id, 5)
end

function Pin_enter(host, me)
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

function Pin_leave(host, me)
    CombatDamageLayers:remove(CardData[me.id].ipID)
end