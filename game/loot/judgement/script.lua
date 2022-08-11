function Judgement_cost(host, cardInfo)
    local players = getPlayers(host)
    local maxS = 0
    local choices = {}
    for _, player in ipairs(players) do
        local sc = Common_SoulCount(player)
        if sc == maxS then
            choices[#choices+1] = player.id
        end
        if sc > maxS then
            choices = {player.id}
            maxS = sc
        end
    end
    if maxS == 0 then
        return false
    end
    if #choices == 1 then
        pushTarget(host, choices[1], PLAYER)
        return true
    end
    local choiceId, _ = requestChoice(host, cardInfo.ownerID, 'Choose a player', PLAYER, choices)
    pushTarget(host, choiceId, PLAYER)
    return true
end

function Judgement_use(host)
    local target = popTarget(host)
    local choice = Common_ChooseSoul(host, target.id, target.id)
    destroySoul(host, target.id, choice)
end
