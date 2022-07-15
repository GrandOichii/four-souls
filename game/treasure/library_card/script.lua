--  TODO untested

function LibraryCard_cost(host, cardInfo)
    return Common_Tap(host)
end

function LibraryCard_tap(host)
    local owner = getOwner(host)
    addPlayableCount(host, owner["id"])
end