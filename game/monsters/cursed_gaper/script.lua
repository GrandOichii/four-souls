function CursedGaper_enter(host)
	local me = this(host)
	local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = CardData[me.id].pInc
                return add + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
    CardData[me.id] = {
        pInc = 0,
        ipID = id
    }
end

function CursedGaper_leave(host)
	local me = this(host)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end


function CursedGaper_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 3)
end

function CursedGaper_trigger1_check(host, me)
	return Common_LastRoll(host, me).value == 4
end

function CursedGaper_trigger1(host)
	local me = this(host)
	CardData[me.id].pInc = CardData[me.id].pInc + 1
end

function CursedGaper_trigger2(host)
	local me = this(host)
	CardData[me.id].pInc = 0
end
