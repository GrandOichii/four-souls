function Berkano_cost(host, info)
    return Common_TargetNonEternalCard(host, info.ownerID)

end

function Berkano_effect(host)
    local target = popTarget(host)
    local owner = getOwner(host, target.id)
    destroyCard(host, target.id)
    if owner ~= nil then
        local schoice = Common_ChooseShopItem(host, owner.id)
        removeFromEverywhere(host, schoice)
        moveToBoard(host, owner.id, schoice)
    end
    pushRefillShop(host)
end
