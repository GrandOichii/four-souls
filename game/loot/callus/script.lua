function Callus_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            if tgtType == PLAYER and tgtID == owner.id then
                return
            end
            DamageFuncStack._et[DamageFuncStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount)
        end
    })
end

function Callus_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)
    DamageFuncStack:remove('damage'..me.id)
end
