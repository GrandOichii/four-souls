--  TODO untested

function PlaydoughCookie_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function PlaydoughCookie_trigger(host)
    local players = getPlayers(host)
    local cards = {}
    local withCounters = {}
    for _, player in ipairs(players) do
        for _, card in ipairs(player.board) do
            if not card.isEternal then
                cards[#cards+1] = card.id
                if card.counters > 0 then
                    withCounters[#withCounters+1] = card.id
                end
            end
        end
    end
    local choice1 = 'Put a counter on a non-eternal item.'
    local choice2 = 'Remove a counter from a non-eternal item.'
    local choice3 = 'Gain 1 c.'
    local choices = {}
    if #cards ~= 0 then
        choices[#choices+1] = choice1
    end
    if #withCounters ~= 0 then
        choices[#choices+1] = choice2
    end
    choices[#choices+1] = choice3
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Choose', choices)
    if choice == choice3 then
        addCoins(host, owner.id, 1)
        return
    end
    local t = {}
    t[choice1] = {cards, 'Choose a card to put a counter on', addCounters}
    t[choice2] = {withCounters, 'Choose a card to remove a counter', removeCounters}
    local cid, _ = requestChoice(host, owner.id, t[choice][2], CARD, t[choice][1])
    t[choice][3](host, cid, 1)
end
