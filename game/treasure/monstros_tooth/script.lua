function MonstrosTooth_trigger_check(host, me)
    local flag = Common_OwnersTurn(host, me.id)
    if not flag then
        return false
    end
    return true
end

function MonstrosToorh_trigger_cost(host, cardInfo)
    local players = getPlayers(host)
    local player = players[math.random(1, #players)]
    pushTarget(host, player.id, PLAYER)
    print(player.id)
    return true
end

function MonstrosTooth_trigger(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    print(player.name)
    local ids = {}
    for _, card in ipairs(player.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return
    end
    local choice, _ = requestChoice(host, player.id, 'Choose a card to sacrifice', CARD, ids)
    destroyCard(host, choice)
end
