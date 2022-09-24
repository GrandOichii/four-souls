function LootCard_enter(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function LootCard_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function LootCard_trigger_effect(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local choice = requestSimpleChoice(host, owner.id, 'Return ${'..me.key..'} to your hand?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    removeFromEverywhere(host, me.id)
    moveToHand(host, owner.id, me.id)
end
