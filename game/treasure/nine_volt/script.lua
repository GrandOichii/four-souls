--  TODO untested

function NineVolt_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function NineVolt_trigger(host)
    local cardID, chose = Common_ChooseTappedCard(host, getTopOwner(host).id, true)
    if not chose then
        return
    end
    rechargeCard(host, cardID)
    cardID, chose = Common_ChooseTappedCard(host, getTopOwner(host).id, true)
    if not chose then
        return
    end
    rechargeCard(host, cardID)
end

