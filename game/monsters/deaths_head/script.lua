function DeathsHead_rewards(host, me, killer)
    lootCards(host, killer.id, 1)
end

function DeathsHead_enter(host, me)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                return 1 + MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
	CardData[me.id] = {
		ipID = id
	}
end

function DeathsHead_leave(host, me)
    print(CardData[me.id].ipID)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
end

function DeathsHead_trigger_check(host)
    return Common_LastDeath(host).type == MONSTER
end

function DeathsHead_trigger(host)
    local me = this(host)
    for _, monster in ipairs(getActiveMonsters(host)) do
        if monster.id == me.id then
            killEntity(host, MONSTER, me.id)
        end
    end
end