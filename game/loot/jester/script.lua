--  TODO untested

function Jester_use(host)
    local owner = getTopOwner(host)
    local cardIDs = {}
    -- for _, player in ipairs(getPlayers(host)) do
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            cardIDs[#cardIDs+1] = card.id
        end
    end
    -- end
    local removeItem = function (item)
        for i, value in ipairs(cardIDs) do
            if item == value then
                table.remove(cardIDs, i)
                return
            end
        end
    end
    local results = {}
    while #cardIDs ~= 0 do
        local choice = requestSimpleChoice(host, owner.id, 'Reroll an item?', {'Yes', 'No'})
        if choice == 'No' then
            break
        end
        local choice, _ = requestChoice(host, owner.id, 'Choose an item to reroll', CARD, cardIDs)
        removeItem(choice)
        results[#results+1] = choice
    end
    for _, id in ipairs(results) do
        Common_RerollItem(host, id)
    end
    discardMe(host, this(host).id)

end