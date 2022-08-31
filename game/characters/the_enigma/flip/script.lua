function Flip_trigger_check(host, me)
    return Common_PostDeathOwnerDied(host, me.id)
end

function Flip_trigger(host)
    local owner = getTopOwner(host)
    local cCard = getCharacterCard(host, owner.id)
    if canFlip(host, cCard.id) then
        flip(host, cCard.id)
        rechargeCharacterCard(host, cCard.id)
        healPlayer(host, owner.id, 0)
    end
    for _, card in ipairs(owner.hand) do
        discardLoot(host, owner.id, card.id)
    end
    lootCards(host, owner.id, 3)
end
