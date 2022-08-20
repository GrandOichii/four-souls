function MaskOfInfamy_enter(host, me)
    local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
				local add = 0
				-- local me_ = Common_MonsterWithID(host, mid)
				if mid == me.id and getMonsterHealth(host, mid) == 1 then
					add = 2
				end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function MaskOfInfamy_leave(host, me)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function MaskOfInfamy_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 1)
end