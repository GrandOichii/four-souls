function Hourglass_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 2    
end

function Hourglass_trigger_cost(host, cardInfo)
    local players = getPlayers(host)
    local ids = {}
    for _, player in pairs(players) do
        local board = player.board
        for _, card in ipairs(board) do
            if not card.tapped then
                ids[#ids+1] = card.id
            end
        end
    end
    if #ids == 0 then
        return false
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Deactivate an item?', {'Yes', 'No'})
    if choice == 'No' then
        return false
    end
    local choiceID, _ = requestChoice(host, cardInfo.ownerID, 'Choose an item to deactivate', CARD, ids)
    pushTarget(host, choiceID, CARD)
    return true
end

function Hourglass_trigger(host)    
    local target = popTarget(host)
    tapCard(host, target.id)
end