--  TODO untested
--  TODO decide the order of the players

function Death_use(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        killEntity(host, PLAYER, player.id)
    end
end