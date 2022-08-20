function Seraphim_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        rCount = 0
    }
end

function Seraphim_trigger1_check(host, me)
    local lr = Common_LastRoll(host)
    local owner = getOwner(host, me.id)
    local flag = lr.ownerID == owner.id and lr.isCombatRoll
    if not flag then
        return false
    end
    CardData[me.id].rCount = CardData[me.id].rCount + 1
    local rCount = CardData[me.id].rCount
    return rCount == 1 or rCount == 3 or rCount == 5
end

function Seraphim_trigger1(host)
    Common_ModLastRoll(host, 1)
end

function Seraphim_trigger2_check(host, me)
    return true
end

function Seraphim_trigger2(host)
    local me = this(host)
    CardData[me.id].rCount = 0
end
