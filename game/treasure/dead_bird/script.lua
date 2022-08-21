function DeadBird_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 3
end

function DeadBird_trigger(host)
    local lr = CardData[this(host).id].lastRoll
    local player = Common_PlayerWithID(host, lr.ownerID)
    local owner = getTopOwner(host)
    if #player.hand == 0 then
        return
    end
    local choice = requestSimpleChoice(host, owner.id, 'Steal a card from '..player.name..'\' hand?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local cid = requestCardsInHand(host, owner.id, player.id, 'Choose a card to steal', 1)[1]
    removeFromEverywhere(host, cid)
    moveToHand(host, owner.id, cid)
end
