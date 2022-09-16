function Betrayal_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function Betrayal_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local roll = popRollStack(host)
    local player = Common_PlayerWithID(host, target.id)
    if roll.value < 3 then
        local amount = math.min(5, player.coins)
        subCoins(host, player.id, amount)
        addCoins(host, owner.id, amount)
    end
    if (roll.value == 3 or roll.value == 4) and #player.hand ~= 0 then
        local cid = player.hand[math.random(1, #player.hand)].id
        removeFromEverywhere(host, cid)
        moveToHand(host, owner.id, cid)
    end
    if roll.value == 5 then
        local ids = {}
        for _, card in ipairs(player.board) do
            if not card.isEternal then
                ids[#ids+1] = card.id
            end
        end
        local choice, _ = requestChoice(host, owner.id, 'Choose an item to steal', CARD, ids)
        removeFromEverywhere(host, choice)
        moveToBoard(host, owner.id, choice)

    end
    if roll.value == 6 and #player.souls ~= 0 then
        local choice = Common_ChooseSoul(host, owner.id, player.id)
        removeFromEverywhere(host, choice)
        addSoulCard(host, owner.id, choice)
    end
    Common_BonusMonsterTail(host, me.id)
end
