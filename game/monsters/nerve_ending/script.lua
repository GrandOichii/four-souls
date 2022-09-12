function NerveEnding_rewards(host)
	local owner = getTopOwner(host)
	addCoins(host, owner.id, 3)
end