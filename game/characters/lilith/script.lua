function Lilith_tap(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end