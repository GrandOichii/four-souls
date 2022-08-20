function LostSoul_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, owner.id, me.id)
end