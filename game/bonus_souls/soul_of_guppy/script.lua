local GUPPY_ITEMS = {
    'Guppy\'s Hairball',
    'Guppy\'s Collar',
    'Guppy\'s Head',
    'Guppy\'s Paw',
    'Guppy\'s Tail',
    'Guppy\'s Eye',
    'The Dead Cat',
}

local function IsGuppyItem(item)
    for _, name in ipairs(GUPPY_ITEMS) do
        if name == item.name then
            return true
        end
    end
    return false
end

function SoulOfGuppy_trigger_check(host, me)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        local gCount = 0
        for _, item in ipairs(player.board) do
            if IsGuppyItem(item) then
                gCount = gCount + 1
            end
        end
        if gCount >= 2 then
            CardData[me.id] = {
                ownerID = player.id
            }
            return true
        end
    end
    return false
end

function SoulOfGuppy_trigger(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, CardData[me.id].ownerID, me.id)
end
