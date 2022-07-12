function TheSun_use(host)
    local owner = getOwner(host)
    local current = getCurrentPlayer(host)
    if owner["id"] ~= current["id"] then 
        return
    end
    setNextPlayer(host, owner["id"])
end