function TheD6_cost(host, cardName)
    --  TODO
    return Common_Tap(host, cardName)
end

function TheD6_tap(host)
    --  TODO
    local owner = getTopOwner(host)
    local choices = {"Smart fella", "Fart smella"}
    local choice = requestSimpleChoice(host, owner["id"], "Choose", choices)
    print("You chose "..choice)
    print("the d6 was activated")
end