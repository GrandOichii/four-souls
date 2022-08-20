function CarBattery_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        activated = false
    }
    local id = 'recharge'..me.id
    RechargeFuncStack:push({
        name = id,
        func = function (host_, cid)
            if cid == me.id then
                if not CardData[me.id].activated then
                    return
                end
                CardData[me.id].activated = false
            end
            RechargeFuncStack._et[RechargeFuncStack:posOf(id)-1].func(host_, cid)
        end
    })
end

function CarBattery_leave(host, me, owner)
    RechargeFuncStack:remove('recharge'..me.id)
end

function CarBattery_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local fetchTapped = function ()
        local o = getTopOwner(host)
        local result = {}
        for _, card in ipairs(o.board) do
            if card.tapped and card.id ~= me.id then
                result[#result+1] = card.id
            end
        end
        return result
    end
    for i = 1, 3 do
        -- TODO
        local ids = fetchTapped()
        if #ids == 0 then
            return
        end
        local choiceId, _ = requestChoice(host, owner.id, "Choose a tapped card", CARD, ids)
        rechargeCard(host, choiceId)
    end
end

function CarBattery_cost2(host, cardInfo)
    return Common_Pay(host, cardInfo.ownerID, 7)
end

function CarBattery_recharge(host)
    local me = this(host)
    CardData[me.id].activated = true
end
