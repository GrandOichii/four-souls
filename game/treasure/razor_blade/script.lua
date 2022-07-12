function RazorBlade_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Tap(host, cardInfo)
end

function RazorBlade_tap(host)
    local target = popTarget(host)
    local opponent = Common_PlayerWithID(host, target["id"])
    dealDamage(host, opponent["id"], 1)
end