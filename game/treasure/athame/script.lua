--  TODO? maybe move the player targeting to the Athame_force function

function Athame_cost(host, cardInfo)
    return Common_Discard(host, cardInfo["ownerID"], 1) and Common_TargetPlayer(host, cardInfo.ownerID)
end

function Athame_force(host)
    local target = popTarget(host)
    Common_Discard(host, target["id"], 2)
end
