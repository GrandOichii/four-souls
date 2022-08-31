function Dinga_enter(host)
	local me = this(host)
	local id = CombatDamageLayers:top().id + 1
    CombatDamageLayers:push(
        {
            id = id,
			func = function (host_, srcType, srcID, tgtType, tgtID, amount, roll)
                if tgtType == MONSTER and tgtID == me.id then
                    CardData[me.id].lcr = roll
				end
				return CombatDamageLayers._et[CombatDamageLayers:posOf(id)-1].func(host_, srcType, srcID, tgtType, tgtID, amount, roll)
            end
        }
    )
	CardData[me.id] = {
		rMult = 1,
		cdID = id,
		lcr = 0
	}
end

function Dinga_leave(host)
	local me = this(host)
    CombatDamageLayers:remove(CardData[me.id].cdID)
end

function Dinga_death_check(host, me)
	return CardData[me.id].lcr == 6
end

function Dinga_death(host)
	local me = this(host)
	CardData[me.id].rMult = 2
end

function Dinga_rewards(host)
	local me = this(host)
	local owner = getTopOwner(host)
	local roll = popRollStack(host)
	addCoins(host, owner.id, CardData[me.id].rMult * roll.value)
end