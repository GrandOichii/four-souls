
function CursedKeeperHead_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	addCoins(host, killer.id, roll.value)
end

function CursedKeeperHead_trigger_check(host, me)
	return Common_LastRoll(host).value == 1
end

function CursedKeeperHead_trigger(host)
	local roll = Common_LastRoll(host)
	-- print('OWNER ID: '..roll.ownerID)
	local owner = Common_PlayerWithID(host, roll.ownerID)
	-- print('OWNER: '..owner.id)
	subCoins(host, owner.id, math.min(2, owner.coins))
end
