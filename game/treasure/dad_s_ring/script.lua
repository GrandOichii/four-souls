--  TODO currently no way to know who declared the attack

function DadsRing_enter(host)
    local me = this(host)
    local id = MonsterRollLayers:top().id + 1
    MonsterRollLayers:push(
        {
            id = id,
            func = function (host_, mid)
                local sub = 0
                for index, value in pairs(CardData[me.id].mids) do
                    if index == mid then
                        sub = value
                        break
                    end
                end
                return math.max(0, MonsterRollLayers._et[MonsterRollLayers:posOf(id)-1].func(host_, mid) - sub)
            end
        }
    )
	CardData[me.id] = {
		irID = id,
        mids = {}
	}
end

function DadsRing_leave(host)
    local me = this(host)
    MonsterRollLayers:remove(CardData[me.id].irID)
end

function DadsRing_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function DadsRing_trigger1(host)
    local roll = popRollStack(host)
    local me = this(host)
    if roll.value < 4 then
        return
    end
    for _, card in pairs(getActiveMonsters(host)) do
        if card.isBeingAttacked then
            if CardData[me.id].mids[card.id] == nil then
                CardData[me.id].mids[card.id] = 0
            end
            CardData[me.id].mids[card.id] = CardData[me.id].mids[card.id] + 1
            return
        end
    end
end

function DadsRing_trigger2(host)
    local me = this(host)
    CardData[me.id].mids = {}
end