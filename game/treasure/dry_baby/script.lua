--  TODO untested

function DryBaby_enter(host, me, owner)
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            if tgtType == PLAYER and tgtID == owner.id then
                amount = 1
            end
            DeathStack._et[DeathStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount)
        end
    })
end

function DryBaby_leave(host, me, owner)
    DeathStack:remove('damage'..me.id)
end
