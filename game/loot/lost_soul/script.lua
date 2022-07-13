function LostSoul_use(host)
    local owner = getTopOwner(host)
    addSouls(host, owner["id"], 1)
end