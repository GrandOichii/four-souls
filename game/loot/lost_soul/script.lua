function LostSoul_use(host)
    local owner = getOwner(host)
    addSouls(host, owner["id"], 1)
end