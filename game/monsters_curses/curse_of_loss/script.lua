function CurseOfLoss_trigger(host)
    local owner = getTopOwner(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    discardMe(host, me.id)
    local choice = Common_ChooseSoul(host, owner.id, owner.id)
    removeFromEverywhere(host, choice)
    discardMe(host, choice)
end