function Lemegeton_cost(host, cardInfo)
    return Common_Tap(host)
end

function Lemegeton_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    plusOneTreasure(host, owner.id)
    if CardData[me.id] == nil then
        CardData[me.id] = {
            usedC = 0
        }
    end
    CardData[me.id].usedC = CardData[me.id].usedC + 1
end

function Lemegeton_trigger_check(host, me)
    if CardData[me.id] == nil then
        CardData[me.id] = {
            usedC = 0
        }
        return false
    end
    local flag = Common_OwnersTurn(host, me.id)
    if not flag then
        return false
    end
    if CardData[me.id].usedC ~= 0 then
        return true
    end
    return false
end

function Lemegeton_trigger(host)
    local me = this(host)
    local owner = getTopOwner(host)
    while CardData[me.id].usedC ~= 0 do
        local ids = {}
        for _, card in ipairs(owner.board) do
            if not card.isEternal then
                ids[#ids+1] = card.id
            end
        end
        if #ids == 0 then
            CardData[me.id].usedC = 0
            return
        end
        local choice, _ = requestChoice(host, owner.id, 'Choose an item to sacrifice', CARD, ids)
        destroyCard(host, choice)
        CardData[me.id].usedC = CardData[me.id].usedC - 1
    end
end

