function TheCurdled_use(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end