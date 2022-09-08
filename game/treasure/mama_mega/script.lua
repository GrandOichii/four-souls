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
    for _, tt in ipairs(t) do
        for _, element in ipairs(tt.collection) do
            print('DEALING DAMAGE TO '..tt.elType..' (ID: '..element.id..')')
            dealDamage(host, PLAYER, owner.id, tt.elType, element.id, 3)
        end
    end
end