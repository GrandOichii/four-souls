function CurseOfImpulse_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CurseOfImpulse_trigger(host)
    local owner = getTopOwner(host)
    for _, card in ipairs(owner.board) do
        tapCard(host, card.id)
    end
    tapCharacterCard(host, owner.id)
end
