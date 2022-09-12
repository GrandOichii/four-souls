--  TODO untested
--  TODO figure out how to use the stack

function OneUP_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    DeathStack:push({
        name = 'death'..me.id,
        func = function (host_, player)
            if player.id ~= owner.id then
                return DeathStack._et[DeathStack:posOf('death'..me.id)-1].func(host_, player)
            end
            cancelCurrentAttack(host_)
            setTurnEnd(host_, false)
            Common_IncAttackCount(host, player.id, false)
            healPlayer(host, player.id, _getMaxHealth(host, player.id))
            destroyCard(host, me.id)
            return false
        end
    })
end

function OneUP_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    DeathStack:remove('death'..me.id)
end
