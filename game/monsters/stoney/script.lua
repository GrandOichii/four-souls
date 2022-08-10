--  TODO

function Stoney_enter(host, me)
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

function Stoney_leave(host, me)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function Stoney_rewards(host, me, killer)
	lootCards(host, killer.id, 1)
end

function Stoney_trigger_check(host)
    return Common_LastDeath(host).type == MONSTER
end

function Stoney_trigger(host)
    local me = this(host)
    for _, monster in ipairs(getActiveMonsters(host)) do
        if monster.id == me.id then
            killEntity(host, MONSTER, me.id)
        end
    end
end