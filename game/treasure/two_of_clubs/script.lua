-- TODO untested

function TwoOfClubs_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        activated = false,
        targetID = -1
    }
    local id = 'loot'..me.id
    LootCardsFuncStack:push({
        name = id,
        func = function (host_, pid, amount)
            local data = CardData[me.id]
            if data.activated and pid == data.targetID then
                amount = 2 * amount
            end
            LootCardsFuncStack._et[LootCardsFuncStack:posOf(id)-1].func(host_, pid, amount)
        end
    })
end

function TwoOfClubs_leave(host, me, owner)
    LootCardsFuncStack:remove('loot'..me.id)
end

function TwoOfClubs_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function TwoOfClubs_tap(host)
    local me = this(host)
    CardData[me.id].activated = true
    CardData[me.id].targetID = popTarget(host).id
end

function TwoOfClubs_trigger_check(host, me)
    return true
end

function TwoOfClubs_trigger(host)
    local me = this(host)
    CardData[me.id].activated = false
    CardData[me.id].targetID = -1
end
