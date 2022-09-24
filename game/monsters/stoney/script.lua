--  TODO

function Stoney_enter(host)
    local me = this(host)

	local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                return math.min(6, 1 + MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid))
            end
        }
    )
	CardData[me.id] = {
		irID = id
	}
end

function Stoney_leave(host)
    local me = this(host)

    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Stoney_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 1)
end

function Stoney_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function Stoney_trigger(host)
    local me = this(host)
    for _, monster in pairs(getActiveMonsters(host)) do
        if monster.id == me.id then
            killEntity(host, MONSTER, me.id)
        end
    end
end