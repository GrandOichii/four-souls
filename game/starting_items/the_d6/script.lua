function TheD6_tap(host)
    --  TODO
    print("the d6 was activated")
end

function TheD6_eot()
    print("END OF TURN")
end

function TheD6_trigger(host)
    local owner = getOwner(host)
    print(owner["name"])
    deferEOT(host, this(host)["id"], "TheD6_eot", false)
end

function TheD6_trigger_check(host)
    return true
end