--  TODO untested
--  TODO figure out how to use the stack

function OneUP_enter(host, me, owner)
    DeathStack:push({
        name = 'death'..me.id,
        func = function (host_, player)
            if player.id ~= owner.id then
                DeathStack._et[DeathStack:posOf('death'..me.id)-1].func(host_, player)
                return
            end
            cancelCurrentAttack(host_)
            setTurnEnd(host_, false)
            incAttackCount(host, player.id)
            healPlayer(host, player.id, _getMaxHealth(host, player.id))
            destroyCard(host, me.id)
        end
    })
end

function OneUP_leave(host, me, owner)
    DeathStack:remove('death'..me.id)
end
