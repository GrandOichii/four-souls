function Hanger_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer["id"], 7)
end

function Hanger_death(host)
	expandShopSize(host, 1)
end