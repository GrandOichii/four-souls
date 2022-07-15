--  TODO untested

function TheButterBean_cost(host, cardInfo)
    return Common_Tap(host) and Common_Roll(host, cardInfo["ownerID"])
end

function TheButterBean_tap(host)
    local card = this(host)
    local roll = getLastRoll(host)
    if roll == 6 then
        rechargeCard(host, card["id"])
    end
end