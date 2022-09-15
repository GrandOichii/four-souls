function Sucker_rewards(host)
    local owner = getTopOwner(host)
end

function Sucker_death(host)
    --  TODO could cause errors, as it is dead when the trigger resolves
    local me = this(host)
    for _, player in ipairs(getPlayers(host)) do
        dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
    end
end
