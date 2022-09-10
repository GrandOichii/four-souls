function Chest_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    local amount = 1
    if roll.value > 3 then
        amount = 3
        if roll.value > 4 then
            amount = 6
        end
    end
    addCoins(host, owner.id, amount)
	Common_BonusMonsterTail(host, me.id)
end