function SpindownDice_cost(host, info)
    return Common_TargetNonEternalCard(host, info.ownerID)
end

function SpindownDice_effect(host)
    local target = popTarget(host)
    local owner = getOwner(host, target.id)
    destroyCard(host, target.id)
    if owner ~= nil then
        local choice = requestSimpleChoice(host, owner.id, 'Steal an item from the shop?', {'Yes', 'No'})
        if choice == 'No' then
            return
        end
        local schoice = Common_ChooseShopItem(host, owner.id)
        removeFromEverywhere(host, schoice)
        moveToBoard(host, owner.id, schoice)
    end
    pushRefillShop(host)
end


function SpindownDice_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function SpindownDice_trigger(host)
    local roll = popRollStack(host)
    if roll.value < 4 then
        return
    end
    local me = this(host)
    rechargeCard(host, me.id)
end
