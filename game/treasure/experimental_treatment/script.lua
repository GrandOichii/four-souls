function ExperimentalTreatment_cost(host, cardInfo)
    local me = this(host)
    destroyCard(host, me.id)
    return Common_Roll(host, cardInfo.ownerID)
end

function ExperimentalTreatment_inject(host)
    local me = this(host)
    local roll = getLastRoll(host).value
    local owner = getTopOwner(host)
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