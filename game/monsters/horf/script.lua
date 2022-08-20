function Horf_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 3)
end

function Horf_enter(host, me)
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
				local add = 0
                if srcType == MONSTER and srcID == me.id and roll == 2 then
					add = 1
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount + add, roll)
            end
        }
    )
    print('ENTERED: '..me.id)
	CardData[me.id] = {
        ipID = id
	}
end

function Horf_leave(host, me)
    print('LEFT: '..me.id)
    print(CardData[me.id])
    CombatDamageLayers:remove(CardData[me.id].ipID)
end