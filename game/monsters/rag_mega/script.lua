--  TODO untested

function RagMega_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function RagMega_death_cost(host, info)
    local me = this(host)
    local cPlayer = getCurrentPlayer(host)
    local ids = {}
    for _, player in ipairs(getPlayers(host)) do
        if player.id ~= cPlayer.id then
            ids[#ids+1] = player.id
        end
    end
    local removeID = function (id_)
        for i, value in ipairs(ids) do
            if value == id_ then
                table.remove(ids, i)
                return
            end
        end
    end
    local amount = math.min(#ids, 3)
    for i = 1, amount do
        local choice, _ = requestChoice(host, cPlayer.id, 'Choose a player to kill', PLAYER, ids)
        removeID(choice)
        pushTarget(host, choice, PLAYER)
    end
    CardData[me.id] = {
        amount = amount
    }
    return true
end

function RagMega_death(host)
    local me = this(host)
    for i = 1, CardData[me.id].amount do
        local target = popTarget(host)
        killEntity(host, PLAYER, target.id)
    end
end

function RagMega_trigger_check(host, me)
    return Common_MonsterWithID(host, me.id).isBeingAttacked
end

function RagMega_trigger(host)
    local cPlayer = getCurrentPlayer(host)
    for _, player in ipairs(getPlayers(host)) do
        if cPlayer.id ~= player.id then
            Common_Discard(host, player.id, 1, false)
        end
    end
end
