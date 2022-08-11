function ConsolationPrize_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function ConsolationPrize_trigger(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    local maxS = 0
    for _, player in ipairs(players) do
        local sc = Common_SoulCount(player)
        if player.id ~= owner.id and sc > maxS then
            maxS = sc
        end
    end
    local amount = maxS - Common_SoulCount(owner)
    if amount <= 0 then
        return
    end
    lootCards(host, owner.id, amount)
end
