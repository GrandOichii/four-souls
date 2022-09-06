function SoulOfGreed_trigger_check(host, me)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.coins >= 25 then
            CardData[me.id] = {
                ownerID = player.id
            }
            return true
        end
    end
    return false
end

function SoulOfGreed_trigger(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    local ownerID = CardData[me.id].ownerID
    addSoulCard(host, ownerID, me.id)
end
