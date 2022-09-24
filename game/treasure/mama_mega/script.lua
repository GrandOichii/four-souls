function MamaMega_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local t = {
        {
            collection = getActiveMonsters(host),
            elType = MONSTER
        },
        {
            collection = getPlayers(host),
            elType = PLAYER
        }
    }
    --  TODO might not work with reworking of getActiveMonsters
    for _, tt in ipairs(t) do
        for _, element in pairs(tt.collection) do
            dealDamage(host, PLAYER, owner.id, tt.elType, element.id, 3)
        end
    end
end