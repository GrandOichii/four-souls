--  TODO unfinished

function BibleThump_use(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
    discardMe(host, this(host).id)

end