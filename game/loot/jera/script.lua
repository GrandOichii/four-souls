function Jera_use(host)
    local owner = getTopOwner(host)
    local count = Common_CardCount(owner)
    lootCards(host, owner["id"], count)
end