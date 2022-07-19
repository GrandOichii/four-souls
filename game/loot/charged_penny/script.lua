function ChargedPenny_use(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 1)
    Common_RechargeAnItem(host, owner.id, false)
end