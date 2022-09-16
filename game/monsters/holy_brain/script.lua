function HolyBrain_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function HolyBrain_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - CardData[me.id].sub)
            end
        }
    )
	CardData[me.id] = {
        sub = 0,
		irID = id
	}
end

function HolyBrain_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function HolyBrain_trigger1_check(host, me)
    return Common_LastRoll(host, me).value == 3
end

function HolyBrain_trigger1(host)
    local me = this(host)
    CardData[me.id].sub = CardData[me.id].sub + 1
end

function HolyBrain_trigger2(host)
    local me = this(host)
    CardData[me.id].sub = 0
end
