function MamaMega_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    destroyCard(host, me.id)
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