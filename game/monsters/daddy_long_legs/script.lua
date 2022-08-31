function DaddyLongLegs_rewards(host)
	local owner = getTopOwner(host)
	addCoins(host, owner.id, 7)
end

function DaddyLongLegs_enter(host)
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

function DaddyLongLegs_leave(host)
	local me = this(host)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end

function DaddyLongLegs_trigger1_check(host, me)
	return Common_MonsterWithID(host, me.id).isBeingAttacked and Common_LastRoll(host, me).value == 1
end

function DaddyLongLegs_trigger1(host)
	local me = this(host)
	CardData[me.id].pInc = CardData[me.id].pInc + 1
end

function DaddyLongLegs_trigger2(host)
	local me = this(host)
	CardData[me.id].pInc = 0
end