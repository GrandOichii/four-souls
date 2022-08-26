function Hemoptysis_enter(host)
    local me = this(host)
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                for _, mmid in ipairs(CardData[me.id].mids) do
                    if mmid == mid then
                        return 1
                    end
                end
                return MonsterPowerLayers._et[MonsterPowerLayers:posOf(id)-1].func(host_, mid)
            end
        }
    )
    CardData[me.id] = {
        mids = {},
        pwrID = id
    }
end

function Hemoptysis_leave(host)
    local me = this(host)
    MonsterPowerLayers:remove(CardData[me.id].pwrID)
end

function Hemoptysis_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Hemoptysis_trigger(host)
    local me = this(host)
    CardData[me.id].mids = {}
end

function Hemoptysis_tap(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function Hemoptysis_cost1(host, info)
    return Common_RemoveCounter(host) and Common_TargetMonster(host, info.ownerID)
end

function Hemoptysis_effect1(host)
    local target = popTarget(host)
    local me = this(host)
    CardData[me.id].mids[#CardData[me.id].mids+1] = target.id
end

function Hemoptysis_cost2(host, info)
    local me = this(host)
    local removed = Common_RemoveCounters(host, 2)
    if not removed then
        return false
    end
    local mids = {}
    for _, monster in ipairs(getActiveMonsters(host)) do
        mids[#mids+1] = monster.id
    end
    local removeM = function (mid)
        for i, id in ipairs(mids) do
            if id == mid then
                table.remove(mids, i)
                return
            end
        end
    end
    local pids = {}
    for _, player in ipairs(getPlayers(host)) do
        pids[#pids+1] = player.id
    end
    local removeP = function (pid)
        for i, id in ipairs(pids) do
            if id == pid then
                table.remove(pids, i)
                return
            end
        end
    end
    CardData[me.id].targetC = 0
    for i = 1, 2 do
        local choice = requestSimpleChoice(host, info.ownerID, 'Deal damage to what?', {'Monster', 'Player', 'Stop'})
        if choice == 'Stop' then
            break
        end
        CardData[me.id].targetC = CardData[me.id].targetC + 1
        if choice == 'Monster' then
            local mchoice, _ = requestChoice(host, info.ownerID, 'Choose a monster', MONSTER, mids)
            removeM(mchoice)
            pushTarget(host, mchoice, MONSTER)
        end
        if choice == 'Player' then
            local pchoice, _ = requestChoice(host, info.ownerID, 'Choose a player', PLAYER, pids)
            removeP(pchoice)
            pushTarget(host, pchoice, PLAYER)
        end
    end
    return true
end

function Hemoptysis_effect2(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local count = CardData[me.id].targetC
    for i = 1, count do
        local target = popTarget(host)
        dealDamage(host, PLAYER, owner.id, target.type, target.id, 1)
    end
end
