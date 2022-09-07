--  TODO move to tageting

function GlassCannon_cost(host, cardInfo)
    local me = this(host)
    if me.tapped then
        return false
    end
    local cardIDs = {}
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if not card.isEternal and me.id ~= card.id then
                cardIDs[#cardIDs+1] = card.id
            end
        end
    end
    if #cardIDs == 0 then
        return false
    end
    local choice, _ = requestChoice(host, cardInfo.ownerID, 'Choose card', CARD, cardIDs)
    destroyCard(host, choice)
    return true
end

function GlassCannon_tap(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local roll = popRollStack(host).value
    if roll == 6 then
        rechargeCard(host, me.id)
        return
    end
    destroyCard(host, me.id)
    lootCards(host, owner.id, 2)
end