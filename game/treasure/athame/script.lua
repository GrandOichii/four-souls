--  TODO untested
--  TODO? maybe move the player targeting to the Athame_force function

function Athame_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Discard(host, cardInfo["id"], 1)
end

function Athame_force(host)
    local target = popTarget(host)
    Common_Discard(host, target["id"], 2)
end
