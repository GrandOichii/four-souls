function TheBaleful_tap(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function TheBaleful_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local count = 0
                    local souls = getSoulsOf(host, owner.id)
                    for _, card in ipairs(souls) do
                        count = count + card.soulCount
                        --  TODO incorrect way of adding loot cards to deck
                    end
                    if count == 1 or count == 2 then
                        add = 1
                    end
                    if count >= 3 then
                        add = 2
                    end
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    return id
end

function TheBaleful_leave(host)
    
end