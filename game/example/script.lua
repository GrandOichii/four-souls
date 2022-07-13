function TheD6_tap(host)
    --  TODO
    print("the d6 was activated")
end

function TheD6_eot(host)
    print("D6 END OF TURN")
end

function TheD6_trigger(host)
    local owner = getTopOwner(host)
    print(owner["name"])
    -- lootCards(host, owner["id"], 2)
    deferEOT(host, this(host)["id"], "TheD6_eot", false)
end

function TheD6_trigger_check(host, me)
    return true
end