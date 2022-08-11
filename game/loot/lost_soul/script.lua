function LostSoul_enter(host, me, owner)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, owner.id, me.id)
end