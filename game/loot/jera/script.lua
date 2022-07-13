function Jera_use(host)
    local owner = getTopOwner(host)
    local cards = owner["hand"]
    local count = 0
    for _ in pairs(cards) do count = count + 1 end
    lootCards(host, owner["id"], count)
end