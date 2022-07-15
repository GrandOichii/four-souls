--  TODO untested

function RazorBlade_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Tap(host)
end

function RazorBlade_tap(host)
    local target = popTarget(host)
    dealDamage(host, target["id"], 1)
end