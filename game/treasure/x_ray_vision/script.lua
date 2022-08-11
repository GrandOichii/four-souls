function XRayVision_cost(host, cardInfo)
    local tapped = Common_Tap(host)
    if not tapped then
        return false
    end
    local chosen = Common_TargetPlayer(host, cardInfo)
    if not chosen then
        return false
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Look at the top of which deck?', {LOOT_DECK, TREASURE_DECK, MONSTER_DECK})
    local me = this(host)
    CardData[me.id] = {
        dChoice = choice
    }
    return true
end

function XRayVision_tap(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand ~= 0 then
        requestCardsInHand(host, owner.id, target.id, 'Looking at '..player.name..'\'s hand (click any card to continue)', 1)        
    end
    local deckT = CardData[me.id].dChoice
    local cards = topCardsOf(host, deckT, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local choice = requestSimpleChoice(host, owner.id, 'Put ${'..card.name..'} at the bottom of the '..deckT..' deck?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    millDeck(host, deckT, 1)
end

function XRayVision_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function XRayVision_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
