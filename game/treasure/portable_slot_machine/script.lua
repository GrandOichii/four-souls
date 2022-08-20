--  TODO untested

function PortableSlotMachine_cost(host, cardInfo)
    return Common_Pay(host, cardInfo.ownerID, 3)
end

function PortableSlotMachine_play(host)
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    if roll == 1 or roll == 2 then
        lootCards(host, owner.id, 1)
        return
    end
    if roll == 3 or roll == 4 then
        addCoins(host, owner.id, 4)
        return
    end
end