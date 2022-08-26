function MagicSkin_effect(host)
    local owner = getTopOwner(host)
    local me = this(host)
    gainTreasure(host, owner.id, 1)
    addCounters(host, me.id, 1)
    local choice = Common_ChooseOpponent(host, owner.id)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, choice, me.id)
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
