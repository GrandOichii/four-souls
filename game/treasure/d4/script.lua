function D4_cost(host, cardInfo)
    local isUntapped = Common_Tap(host)
    if not isUntapped then
        return false
    end
    local card = this(host)
    CardData[card.id] = {
        ownerID = cardInfo.ownerID
    }
    destroyCard(host, card.id)
    return true
end

function D4_tap(host)
    local card = this(host)
    local owner = Common_PlayerWithID(host, CardData[card.id].ownerID)
    local choice = Common_ChoosePlayer(host, owner.id)
    local target = Common_PlayerWithID(host, choice)
    local count = 0
    for _, item in ipairs(target.board) do
        if not item.isEternal then
            count = count + 1
            destroyCard(host, item.id)
        end
    end
    for i = 1, count do
        plusOneTreasure(host, target.id)
    end
end