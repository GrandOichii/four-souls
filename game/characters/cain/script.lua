function Cain_tap(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function Cain_game_start(host)
    local owner = getTopOwner(host)
    setCurrentPlayer(host, owner.id)
end