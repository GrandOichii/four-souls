function KeepersBargain_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    decTreasureCost(host, owner.id, 3)
end

function KeepersBargain_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)
    incTreasureCost(host, owner.id, 3)
end

function KeepersBargain_tap(host)
    -- local players = getPlayers(host)
    -- local owner = getTopOwner(host)
    -- local count = 1
    -- for _, player in ipairs(players) do
    --     if owner.id ~= player.id then
    --         local choice = requestSimpleChoice(host, player.id, 'Gain 1$?', {'Yes', 'No'})
    --         if choice == 'Yes' then
    --             addCoins(host, player.id, 1)
    --             count = count + 1
    --         end
    --     end
    -- end
    -- addCoins(host, owner.id, count)
end
