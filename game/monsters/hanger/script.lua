function Hanger_rewards(host, me, killer)
	addCoins(host, killer["id"], 7)
end

function Hanger_death(host)
	expandShopSize(host, 1)
end