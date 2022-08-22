--  TODO unfinished

function BibleThump_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)

end