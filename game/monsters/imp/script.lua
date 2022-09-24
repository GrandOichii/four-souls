function Imp_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    lootCards(host, owner.id, roll.value)
end

function Imp_trigger1_check(host, me)
    return Common_AttackingPlayerRolled(host, me, 3)
end

function Imp_trigger1(host)
    local me = this(host)
    CardData[me.id].add = CardData[me.id].add + 1
end

function Imp_trigger2(host)
    local me = this(host)
    CardData[me.id].add = 0
end

function Imp_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if mid == me.id then
                    add = CardData[me.id].add
                end
                return math.min(6, add + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id,
        add = 0
	}
end

function Imp_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end
