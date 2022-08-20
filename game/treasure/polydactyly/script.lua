--  TODO experimental

function Polydactyly_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id and CardData[me.id].rolls == 1 then
                    add = 1
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    CardData[me.id] = {
        iaID = id,
        turnC = -1,
        rolls = 0
    }
    addPlayableCount(host, owner.id)
end

function Polydactyly_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end

function Polydactyly_trigger1_check(host, me)
    local roll = Common_LastRoll(host)
    return roll.isCombatRoll
end

function Polydactyly_trigger1(host)
    local turnC = getTurnCounter(host)
    local me = this(host)
    if not Common_OwnersTurn(host, me.id) then
        CardData[me.id].turnC = turnC
        CardData[me.id].rolls = 0
        return
    end
    if CardData[me.id].turnC ~= turnC then
        CardData[me.id].turnC = turnC
        CardData[me.id].rolls = 1
        return
    end
    CardData[me.id].rolls = CardData[me.id].rolls + 1
end

function Polydactyly_trigger2_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Polydactyly_trigger2(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function Polydactyly_trigger3_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Polydactyly_trigger3(host)
    local me = this(host)
    CardData[me.id].rolls = CardData[me.id].rolls + 1
end
