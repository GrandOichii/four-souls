function D4_cost(host, info)
   return Common_TargetPlayer(host, info.ownerID) 
end

function D4_tap(host)
    local me = this(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local targetID = popTarget(host).id
    local target = Common_PlayerWithID(host, targetID)
    local count = 0
    for _, item in ipairs(target.board) do
        if not item.isEternal then
            count = count + 1
            destroyCard(host, item.id)
        end
    end
    gainTreasure(host, target.id, count)
end