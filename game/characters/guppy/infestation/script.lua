function Infestation_tap(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
    Common_Discard(host, owner.id, 1, false)
end