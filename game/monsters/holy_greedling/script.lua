function HolyGreedling_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    addCoins(host, owner.id, roll.value)
end

function HolyGreedling_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function HolyGreedling_trigger(host)
    local player = getCurrentPlayer(host)
    local choice = requestSimpleChoice(host, player.id, 'Gain 3$ ?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    addCoins(host, player.id, 3)
end
