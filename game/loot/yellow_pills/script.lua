function YellowPills_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        addCoins(host, owner.id, 4)
        return
    end
    if roll == 3 or roll == 4 then
        addCoins(host, owner.id, 7)
        return
    end
    if roll == 5 or roll == 6 then
        local amount = owner.coins
        if amount > 4 then
            amount = 4
        end
        subCoins(host, owner.id, amount)
        return
    end

end