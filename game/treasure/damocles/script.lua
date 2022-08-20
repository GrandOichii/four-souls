function Damocles_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    GainTreasureFuncStack:push({
        name = 'gain'..me.id,
        func = function (host_, pid, amount)
            print('GAINING: '..pid..'  OWNER: '..owner.id)
            if pid == owner.id then
                amount = amount + 1
            end
            GainTreasureFuncStack._et[GainTreasureFuncStack:posOf('gain'..me.id)-1].func(host_, pid, amount)
        end
    })

end

function Damocles_leave(host, me, owner)
    GainTreasureFuncStack:remove('gain'..me.id)
end

function Damocles_trigger_check(host, cardInfo)
    return Common_OwnerDied(host, cardInfo.ownerID)
end

function Damocles_trigger(host)
    local me = this(host)
    destroyCard(host, me.id)
end
