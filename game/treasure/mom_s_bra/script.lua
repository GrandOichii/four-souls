--  TODO untested

function MomsBra_enter(host)
    local me = this(host)
    local checkF = function (type, id)
        if type == PLAYER then
            for i, pid in ipairs(CardData[me.id].pids) do
                if pid == id then
                    table.remove(CardData[me.id].pids, i)
                    return true
                end
            end
            return false
        end
        if type == MONSTER then
            for i, mid in ipairs(CardData[me.id].mids) do
                if mid == id then
                    table.remove(CardData[me.id].mids, i)
                    return true
                end
            end

            return false
        end
        return false
    end
    local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if checkF(tgtType, tgtID) then
                    amount = 1
                end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            if checkF(tgtType, tgtID) then
                amount = 1
            end
            DamageFuncStack._et[DamageFuncStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount)
        end
    })
    CardData[me.id] = {
        mids = {},
        pids = {},
        cdID = id
    }
end

function MomsBra_leave(host)
    local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].cdID)
    DamageFuncStack:remove('damage'..me.id)
end

function MomsBra_trigger(host)
    local me = this(host)
    CardData[me.id].pids = {}
    CardData[me.id].mids = {}
end

function MomsBra_cost(host, info)
    return Common_TargetMonsterOrPlayer(host, info.ownerID)
end

function MomsBra_effect(host)
    local target = popTarget(host)
    local me = this(host)
    if target.type == MONSTER then
        CardData[me.id].mids[#CardData[me.id].mids+1] = target.id
        return
    end
    CardData[me.id].pids[#CardData[me.id].mids+1] = target.id
end
