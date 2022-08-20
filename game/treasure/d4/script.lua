function D4_cost(host, cardInfo)
    local card = this(host)
    CardData[card.id] = {
        ownerID = cardInfo.ownerID
    }
    destroyCard(host, card.id)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function D4_tap(host)
    local card = this(host)
    local owner = Common_PlayerWithID(host, CardData[card.id].ownerID)
    local targetID = popTarget(host).id
    local target = Common_PlayerWithID(host, targetID)
    local count = 0
    for _, item in ipairs(target.board) do
        if not item.isEternal then
            count = count + 1
            destroyCard(host, item.id)
        end
    end
    gainTreasure(host, target.id, count)
end