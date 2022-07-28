function TheHabit_trigger_check(host, me)
    -- TODO if encountered "damage stack not empty error", uncomment this
    -- local turnC = getTurnCounter(host)
    -- local cardID = me.id
    -- if CardData[cardID] == nil then
    --     CardData[cardID] = {
    --         lastTurn = -1
    --     }
    -- end
    -- if CardData[cardID].lastTurn == turnC then
    --     return false
    -- end
    -- local flag = Common_OwnerDamaged(host, me.id)
    -- if not flag then
    --     return false
    -- end
    -- CardData[cardID].lastTurn = turnC
    -- return true
    return Common_OwnerDamaged(host, me.id) and Common_OncePerTurn(host, me.id)
end

function TheHabit_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    local choice, chosen = Common_ChooseTappedCard(host, owner.id)
    if not chosen then
        return
    end
    rechargeCard(host, choice)
end