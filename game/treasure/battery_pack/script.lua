function BatteryPack_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function BatteryPack_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function BatteryPack_cost(host, cardInfo)
    return Common_RemoveCounters(host, 2) and Common_TargetPlayer(host, cardInfo)
end

function BatteryPack_recharge(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    for _, card in ipairs(player.board) do
        rechargeCard(host, card.id)
    end
end
