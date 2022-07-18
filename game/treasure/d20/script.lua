function D20_cost(host, cardInfo)
    return Common_Tap(host)
end

function D20_tap(host)
    local owner = getTopOwner(host)
    local targetID, chose = Common_ChooseNonEternalCard(host, owner.id)
    if not chose then
        return
    end
    Common_Reroll(host, targetID)
end