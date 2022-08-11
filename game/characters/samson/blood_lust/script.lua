function BloodLust_cost(host, cardInfo)
    local tapped = Common_Tap(host)
    if not tapped then
        return false
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Increase power of what?', {'Player', 'Monster'})
    if choice == 'Player' then
        return Common_TargetPlayer(host, cardInfo)
    end
    if choice == 'Monster' then
        return Common_TargetMonster(host, cardInfo.ownerID)
    end
end

function BloodLust_tap(host)
    local me = this(host)
    local target = popTarget(host)
    if target.type == PLAYER then
        local data = CardData[me.id].pids
        if data[target.id] == nil then
            CardData[me.id].pids[target.id] = 0
        end
        CardData[me.id].pids[target.id] = CardData[me.id].pids[target.id] + 1
        return
    end
    if target.type == MONSTER then
        local data = CardData[me.id].mids
        if data[target.id] == nil then
            CardData[me.id].mids[target.id] = 0
        end
        CardData[me.id].mids[target.id] = CardData[me.id].mids[target.id] + 1
        return
    end
end

function BloodLust_enter(host, me, owner)
    -- monster power
    local id = MonsterPowerLayers:top().id + 1
    MonsterPowerLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local add = 0
                if CardData[me.id].mids[mid] ~= nil then
                    add = CardData[me.id].mids[mid]
                end
                local pos = MonsterPowerLayers:posOf(id)-1
                return add + MonsterPowerLayers._et[pos].func(host_, mid)
            end
        }
    )
    CardData[me.id] = {
        mids = {},
        pids = {},
        ipID = id
    }
    -- player power
    local aid = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = aid,
            func = function (host_, pid)
                local add = 0
                if CardData[me.id].pids[pid] ~= nil then
                    add = CardData[me.id].pids[pid]
                end
                return add + AttackLayers._et[AttackLayers:posOf(aid)-1].func(host_, pid)
            end
        }
    )
    CardData[me.id].iaID = id
end

function BloodLust_leave(host, me, owner)
    MonsterPowerLayers:remove(CardData[me.id].ipID)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end

function BloodLust_trigger_check(host, me)
    CardData[me.id] = {
        mids = {},
        pids = {}
    }
    return Common_OwnersTurn(host, me.id)
end

function BloodLust_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
