function DankGlobin_rewards(host, me, killer)
	lootCards(host, killer["id"], 2)
end

function DankGlobin_death(host)
	local killerID = getLastKillerID(host)
	local choice = Common_ChoosePlayer(host, killerID)
	Common_Discard(host, choice, 2)
end