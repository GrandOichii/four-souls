function Algiz_cost(host, info)
    return Common_TargetPlayer(host, info.ownerID)
end

function Algiz_effect(host)
    --  TODO could cause problems if multiple effects make thos items eternal, possible soulution: change isEternal from bool to int counters
    local target = popTarget(host)
    local me = this(host)
    local player = Common_PlayerWithID(host, target.id)
    local ids = {}
    for _, card in ipairs(player.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
            setIsEternal(host, card.id, true)
        end
    end
    CardData[me.id] = {
        ids = ids
    }
    deferEOT(host, me.id, 'Algiz_eot', true)
    discardMe(host, me.id)
end

function Algiz_eot(host)
    local me = this(host)
    for _, id in ipairs(CardData[me.id].ids) do
        setIsEternal(host, id, false)
    end
end