--  TODO untested

function RKey_enter(host)
    local me = this(host)
    tapCard(host, me.id)
end

function RKey_effect(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        -- discard souls
        for _, soul in ipairs(player.souls) do
            removeFromEverywhere(host, soul.id)
            discardMe(host, soul.id)
        end
        -- discard hand
        for _, card in ipairs(player.hand) do
            discardLoot(host, player.id, card.id)
        end
        lootCards(host, player.id, 3)
    end
end