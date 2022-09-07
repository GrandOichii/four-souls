function ExperimentalTreatment_inject(host)
    local me = this(host)    
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local t = {
        function ()
            killEntity(host, PLAYER, owner.id)
        end,
        function ()
            lootCards(host, owner.id, 3)
        end,
        function ()
            addCoins(host, owner.id, 9)
        end,
        function ()
            gainTreasure(host, owner.id, 1)
        end,
        function ()
            gainTreasure(host, owner.id, 2)
        end,
        function ()
            removeFromEverywhere(host, me.id)
            addSoulCard(host, owner.id, me.id)
        end
    }
    t[roll]()
end