function MrBoom_cost(host, cardInfo)
    return Common_TargetMonster(host, cardInfo["ownerID"])
end

function MrBoom_tap(host)
    local owner = getTopOwner(host)
    local monster, has = Common_popMonsterTarget(host)
    if not has then
        return
    end
    dealDamage(host, PLAYER, owner["id"], MONSTER, monster["id"], 1)
end