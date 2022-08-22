function MegaBattery_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function MegaBattery_use(host)
    discardMe(host, this(host).id)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    for _, card in ipairs(player.board) do
        rechargeCard(host, card.id)
    end
    

end