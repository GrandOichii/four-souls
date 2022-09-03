function Prethro_cost(host, cardInfo)
    return Common_TargetNonEternalCard(host, cardInfo.ownerID)
end

function Perthro_use(host)
    discardMe(host, this(host).id)

    local target = popTarget(host)
    local choice = target.id
    Common_RerollItem(host, choice)

end