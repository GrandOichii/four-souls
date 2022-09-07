function PageantBoy_tap(host)
    local me = this(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 4 * roll)
end