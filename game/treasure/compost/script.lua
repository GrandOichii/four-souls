--  TODO untested

function Compost_enter(host)
	local me = this(host)
    CardData[me.id] = {
        activated = false
    }
    local id = 'loot'..me.id
    LootCardsFuncStack:push({
        name = id,
        func = function (host_, pid, amount)
            local data = CardData[me.id]
            if data.activated then
                local discard = getDiscard(host)
                amount = math.min(amount, #discard)
                for i = 1, amount do
                    local card = discard[i]
                    removeFromEverywhere(host, card.id)
                    moveToHand(host, pid, card.id)
                end
                CardData[me.id].activated = false
                return
            end
            LootCardsFuncStack._et[LootCardsFuncStack:posOf(id)-1].func(host_, pid, amount)
        end
    })
end

function Compost_leave(host)
    local me = this(host)
    LootCardsFuncStack:remove('loot'..me.id)
end

function Compost_tap(host)
    local me = this(host)
    CardData[me.id].activated = true
end

function Compost_trigger(host)
    local me = this(host)
    CardData[me.id].activated = false
end