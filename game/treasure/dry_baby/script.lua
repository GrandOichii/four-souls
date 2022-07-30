--  TODO untested
--  TODO combat damage is still the same, solution: create _dealCombatDamage function in match, then turn it into a func stack

function DryBaby_enter(host, me, owner)
    local key = 'damage'..me.id
    DamageFuncStack:push({
        name = key,
        func = function (host_, srcType, srcID, tgtType, tgtID, amount)
            if tgtType == PLAYER and tgtID == owner.id then
                amount = 1
            end
            DamageFuncStack._et[DamageFuncStack:posOf(key)-1].func(host_, srcType, srcID, tgtType, tgtID, amount)
        end
    })
end

function DryBaby_leave(host, me, owner)
    DamageFuncStack:remove('damage'..me.id)
end
