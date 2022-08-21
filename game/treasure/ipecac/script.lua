--  TODO untested

function Ipecac_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 1)
    }
end

function Ipecac_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end


function Ipecac_trigger_check(host, cardInfo)
    local roll = Common_LastRoll(host, me)
    return roll.value == 6 and roll.ownerID == cardInfo.ownerID and roll.isCombatRoll
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