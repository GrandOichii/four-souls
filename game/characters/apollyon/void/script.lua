function Void_effect0(host)
    local owner = getTopOwner(host)
    local cCount = #owner.hand
    for _, card in ipairs(owner.hand) do
    discardLoot(host, owner.id, card.id)
    end
    owner = Common_PlayerWithID(host, owner.id)
    lootCards(host, owner.id, cCount - #owner.hand)
end

function Void_cost1(host, info)
    return Common_TargetNonAttackedMonster(host, info.ownerID)
end

function Void_effect1(host)
    local target = popTarget(host)
    -- print('AMOGUS'..getMonsterPiles(host))
    for _, pile in ipairs(getMonsterPiles(host)) do
        print(#pile)
        if #pile ~= 0 and pile[#pile].id == target.id then
            removeFromEverywhere(host, target.id)
            pushRefillMonsters(host)
            discardMe(host, target.id)
            return
        end
    end
end

function Void_cost2(host, info)
    return Common_TargetShopItem(host, info.ownerID)
end

function Void_effect2(host)
    local target = popTarget(host)
    for _, card in ipairs(getShop(host)) do
        if card.id == target.id then
            removeFromEverywhere(host, target.id)
            pushRefillShop(host)
            discardMe(host, target.id)
            return
        end
    end
end