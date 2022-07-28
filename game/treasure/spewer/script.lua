--  TODO unfinished
-- figure out how to pop multiple dice

function Spewer_cost(host, cardInfo)
    local ownerID = cardInfo.ownerID
    local me = this()
    local owner = Common_PlayerWithID(host, ownerID)
    if #owner.hand == 0 then
        return false
    end
    local message = "Choose a card to discard"
    local cardIDs = requestCardsInHand(host, ownerID, ownerID, message, 1)
    local rollAmount = 1
    for _, card in ipairs(owner.hand) do
        if card.id == cardIDs[1] then
            if string.find(card.name, "Pills!") then
                rollAmount = 3
            end
            break
        end
    end
    for i = 1, rollAmount do
        Common_Roll(host, owner.id)
    end
    discardLoot(host, ownerID, cardIDs[1])
    CardData[me.id] = {
        rollAmount = rollAmount
    }
    return true
end

function Spewer_eat(host)
    local me = this(host)
end