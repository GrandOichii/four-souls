function Sumptorium_enter(host, me, owner)
    local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local data = CardData[me.id].mids
                local sub = 0
                if data[mid] ~= nil then
                    sub = data[mid]
                end
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - sub)
            end
        }
    )
    CardData[me.id] = {
        mids = {},
        irID = id
    }
end

function Sumptorium_leave(host, me, owner)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end

function Sumptorium_trigger1_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function Sumptorium_trigger1(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function Sumptorium_trigger2_check(host, me)
    return true
end

function Sumptorium_trigger2(host)
    local me = this(host)
    CardData[me.id].mids = {}
end

function Sumptorium_cost(host, cardInfo)
    return Common_RemoveCounters(host, 2) and Common_TargetMonster(host, cardInfo.ownerID)
end

function Sumptorium_use(host)
    local me = this(host)
    local target = popTarget(host)
    if CardData[me.id].mids[target.id] == nil then
        CardData[me.id].mids[target.id] = 0
    end
    CardData[me.id].mids[target.id] = CardData[me.id].mids[target.id] + 1
end
