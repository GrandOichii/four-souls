function ASack_use(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 3)
end