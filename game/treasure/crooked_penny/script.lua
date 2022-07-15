--  TODO untested

function CrookedPenny_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Tap(host)
end

function CrookedPenny_tap(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target["id"])
    local roll = getLastRoll(host)["value"]
    if roll < 4 then
        addCoins(host, player["id"], player["coins"])
        return
    end
    subCoins(host, player["id"], player["coins"])
end