--  TODO untested

function Ipecac_enter(host, me, owner)
    Common_IncAttack(me.id, owner.id, 1)
end

function Ipecac_leave(host, me, owner)
    Common_DecAttack(me.id)
end


function Ipecac_trigger_check(host, cardInfo)
    local roll = Common_LastRoll(host)
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