function SoulOfGluttony_trigger_check(host, me)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if #player.hand >= 4 then
            CardData[me.id] = {
                ownerID = player.id
            }
            return true
        end
    end
    return false
end

function SoulOfGluttony_trigger(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, CardData[me.id].ownerID, me.id)
end
