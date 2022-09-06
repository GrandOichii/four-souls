--  TODO untested

function CreateStrawberryData(host, meID)
    CardData[meID] = {
        pmap = {}
    }
    for _, player in ipairs(getPlayers(host)) do
        CardData[meID].pmap[player.id] = 0
    end
    print('created strawberry data')
end

function Strawberry_trigger1_check(host, me)
    if CardData[me.id] == nil then
        CreateStrawberryData(host, me.id)
    end
    local roll = Common_LastRoll(host, me)
    if roll.value == 1 then
        CardData[me.id].pmap[roll.ownerID] = CardData[me.id].pmap[roll.ownerID] + 1
        if CardData[me.id].pmap[roll.ownerID] >= 3 then
            CardData[me.id].ownerID = roll.ownerID
            return true
        end
    end
    return false
end

function Strawberry_trigger1(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, CardData[me.id].ownerID, me.id)
end

function Strawberry_trigger2(host)
    local me = this(host)
    CreateStrawberryData(host, me.id)
end
