function TheSun_use(host)
    local owner = getTopOwner(host)
    local current = getCurrentPlayer(host)
    if owner["id"] ~= current["id"] then 
        return
    end
    setNextPlayer(host, owner["id"])
    discardMe(host, this(host).id)

end