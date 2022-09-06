function SoulOfSloth_trigger_check(host, me)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if #player.board == 4 then
            return true
        end
    end
    return false
end

function SoulOfSloth_trigger_cost(host, info)
    local me = this(host)
    local owner = getCurrentPlayer(host)
    local players = getPlayers(host)
    local minC = nil
    local ids = {}
    for _, player in ipairs(players) do
        if minC == nil or #player.board < minC then
            minC = #player.board
            ids = {}
        end
        if #player.board == minC then
            ids[#ids+1] = player.id
        end
    end
    if #ids == 1 then
        pushTarget(host, ids[1], PLAYER)
        return true
    end
    local choice, _ = requestChoice(host, owner.id, 'Choose a player to gain ${'..me.name..'}', PLAYER, ids)
    pushTarget(host, choice, PLAYER)
    return true
end

function SoulOfSloth_trigger(host)
    local me = this(host)
    local target = popTarget(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, target.id, me.id)
end
