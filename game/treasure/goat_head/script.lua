function GoatHead_trigger_check(host, me)
    return Common_OwnersTurn(host, me["id"])
end

function GoatHead_trigger(host)
    local owner = getTopOwner(host)
    local cardC = #(owner.hand)
    local choice = requestSimpleChoice(host, owner["id"], "Discard a card?", {"Yes", "No"})
    local counter = 0
    while cardC ~= 0 and choice == "Yes" and #(getTopOwner(host)["hand"]) ~= 0 do
        counter = counter + 1
        Common_Discard(host, owner["id"], 1)
        choice = requestSimpleChoice(host, owner["id"], "Discard another card?", {"Yes", "No"})
        cardC = cardC - 1
    end
    lootCards(host, owner["id"], counter)
end