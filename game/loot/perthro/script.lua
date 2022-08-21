function Prethro_cost(host, cardInfo)
    return Common_TargetNonEternalCard(host, cardInfo.ownerID)
end

function Perthro_use(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local choice = target.id
    Common_RerollItem(host, choice)
    discardMe(host, this(host).id)

end