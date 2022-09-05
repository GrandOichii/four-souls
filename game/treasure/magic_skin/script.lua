function MagicSkin_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function MagicSkin_effect(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local target = popTarget(host)
    gainTreasure(host, owner.id, 1)
    addCounters(host, me.id, 1)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, target.id, me.id)
end

function MagicSkin_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function MagicSkin_trigger(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    local me = this(host)
    if roll.value >= me.counters then
        return
    end
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            destroyCard(host, card.id)
        end
    end
end
