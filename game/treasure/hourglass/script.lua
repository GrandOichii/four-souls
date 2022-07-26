--  TODO untested

function Hourglass_trigger_check(host, me)
    return Common_LastRoll(host).value == 2
end

function Hourglass_trigger(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    local ids = {}
    for _, player in pairs(players) do
        local board = player["board"]
        for _, card in ipairs(board) do
            if not card.tapped then
                ids[#ids+1] = card.id
            end
        end
    end
    if #ids == 0 then
        return false
    end
    local choiceId, payed = requestChoice(host, owner.id, "Choose a card to deactivate", CARD, ids)
    if not payed then
        return false
    end
    tapCard(host, choiceId)
end