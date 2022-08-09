function KeepersBargain_enter(host, me, owner)
    decTreasureCost(host, owner.id, 3)
end

function KeepersBargain_leave(host, me, owner)
    incTreasureCost(host, owner.id, 3)
end

function KeepersBargain_cost(host, cardInfo)
    return Common_Tap(host)
end

function KeepersBargain_tap(host)
    local players = getPlayers(host)
    local owner = getTopOwner(host)
    local count = 1
    for _, player in ipairs(players) do
        if owner.id ~= player.id then
            local choice = requestSimpleChoice(host, player.id, 'Gain 1$?', {'Yes', 'No'})
            if choice == 'Yes' then
                addCoins(host, player.id, 1)
                count = count + 1
            end
        end
    end
    addCoins(host, owner.id, count)
end
