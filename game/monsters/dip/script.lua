--  TODO

function Dip_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    addCoins(host, killer["id"], 1)
end