function EmptyVessel_enter(host, me, owner)
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local o = Common_PlayerWithID(host, owner.id)
                    if #o.hand == 0 then
                        add = 1
                    end
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    CardData[me.id] = {
        iaID = id
    }
end

function EmptyVessel_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end

function EmptyVessel_trigger_check(host, me)
    local roll = Common_LastRoll(host)
    local owner = getOwner(host, me.id)
    return roll.ownerID == owner.id and owner.coins == 0 and roll.isCombatRoll
end

function EmptyVessel_trigger(host)
    Common_ModLastRoll(host, 1)
end