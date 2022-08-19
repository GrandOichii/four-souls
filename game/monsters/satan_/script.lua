function Satan_rewards(host, me, killer)
    gainTreasure(host, killer.id, 2)
end

function Satan_trigger_check(host, me)
    local lr = Common_LastRoll(host)
    local flag = lr.value == 6 and lr.isCombatRoll
    if not flag then
        return false
    end
    CardData[me.id] = {
        pid = lr.ownerID
    }
    return true
end

function Satan_trigger_cost(host, cardInfo)
    local me = this(host)
    return Common_TargetLivingPlayer(host, CardData[me.id].pid)
end

function Satan_trigger(host)
    local target = popTarget(host).id
    killEntity(host, PLAYER, target)
end
