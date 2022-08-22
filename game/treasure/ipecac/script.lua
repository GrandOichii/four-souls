--  TODO untested

function Ipecac_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 1)
    }
end

function Ipecac_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end


function Ipecac_trigger_check(host, me)
    local roll = Common_LastRoll(host, me)
    return roll.value == 6 and roll.ownerID == me.ownerID and roll.isCombatRoll
end

function Ipecac_trigger(host)
local owner = getTopOwner(host)
local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.id ~= owner.id then
            dealDamage(host, PLAYER, owner.id, PLAYER, player.id, 1)
        end
    end
end