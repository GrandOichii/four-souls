function Finger_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 2
end

function Finger_trigger_cost(host, info)
    local me = this(host)
    local owner = Common_PlayerWithID(host, info.ownerID)
    local rOwnerID = CardData[me.id].lastRoll.ownerID
    local rOwner = Common_PlayerWithID(host, rOwnerID)
    local rIds = {}
    for _, card in ipairs(rOwner.board) do
        if not card.isEternal then
            rIds[#rIds+1] = card.id
        end
    end
    if #rIds == 0 then
        return false
    end
    local ids = {}
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return false
    end
    local choice, _ = requestChoice(host, info.ownerID, 'Choose an item '..rOwner.name..' controls to swap', CARD, rIds)
    pushTarget(host, choice, CARD)
    choice, _ = requestChoice(host, info.ownerID, 'Choose an item you control to swap', CARD, ids)
    pushTarget(host, choice, CARD)
    return true
end

function Finger_trigger(host)
    local target = popTarget(host)
    local rTarget = popTarget(host)
    local owner = getOwner(host, target.id)
    if owner == nil then
        return
    end
    local rOwner = getOwner(host, rTarget.id)
    if rOwner == nil then
        return
    end
    if target.id == rTarget.id then
        return
    end
    local choice = requestSimpleChoice(host, getTopOwner(host).id, 'Exchange control?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    removeFromEverywhere(host, target.id)
    removeFromEverywhere(host, rTarget.id)
    moveToBoard(host, owner.id, rTarget.id)
    moveToBoard(host, rOwner.id, target.id)
end
