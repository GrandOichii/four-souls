function CharmedGreedling_rewards(host)
    --  TODO? move to a different trigger
    local owner = getTopOwner(host)
    addCoins(host, owner.id)
    local playerID = Common_ChooseOpponent(host, owner.id)
    addCoins(host, playerID, 10)
end
