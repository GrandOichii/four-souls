function BoxingGlove_cost(host, info)
    return Common_TargetNonAttackedMonster(host, info.ownerID)
end

function BoxingGlove_tap(host)
    local target = popTarget(host)
    local cid = target.id
    removeFromEverywhere(host, cid)
    placeOnTop(host, MONSTER_DECK, cid)
    putFromTopToBottom(host, MONSTER_DECK, 1)
    pushRefillMonsters(host)
end

function BoxingGlove_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function BoxingGlove_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
