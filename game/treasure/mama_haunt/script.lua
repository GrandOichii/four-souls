function MamaHaunt_enter(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local id = RechargeActivePlayerLayers:top().id + 1
    RechargeActivePlayerLayers:push(
        {
            id = id,
			func = function (host_)
                if getCurrentPlayer(host).id ~= owner.id then
                    RechargeActivePlayerLayers._et[RechargeActivePlayerLayers:posOf(id)-1].func(host_)
                end
            end
        }
    )
	CardData[me.id] = {
		rID = id
	}
end

function MamaHaunt_leave(host)
    local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].rID)
end
