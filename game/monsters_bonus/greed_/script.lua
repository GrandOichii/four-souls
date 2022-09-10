function Greed_cost(host, info)
    local players = getPlayers(host)
    local maxC = 0
    local ids = {}
    for _, player in ipairs(players) do
        if player.coins > maxC then
            maxC = player.coins
            ids = {}
        end
        if maxC == player.coins then
            ids[#ids+1] = player.id
        end
    end
    if #ids == 1 then
        pushTarget(host, ids[1], PLAYER)
        return true
    end
    local target, _ = requestChoice(host, info.ownerID, 'Choose the player who will lose all their $.', PLAYER, ids)
    pushTarget(host, target, PLAYER)
    return true
end

function Greed_effect(host)
    local me = this(host)
    local target = popTarget(host)
    subCoins(host, target.id, target.coins)
    Common_BonusMonsterTail(host, me.id)
end