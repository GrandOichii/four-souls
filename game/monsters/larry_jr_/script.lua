function LarryJr_enter(host, me)
	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
				local add = 0
				if mid == me.id and getMonsterHealth(host, mid) <= 2 then
					add = 1
				end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function LarryJr_leave(host, me)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function LarryJr_rewards(host, me, killer)
	addCoins(host, killer.id, 6)
end