function D20_cost(host, cardInfo)
    return Common_Tap(host)
end

function D20_tap(host)
    local owner = getTopOwner(host)
    local targetID, chose = Common_ChooseNonEternalCard(host, owner.id)
    if not chose then
        return
    end
    owner = getOwner(host, targetID)
    destroyCard(host, targetID)
    plusOneTreasure(host, owner.id)
end