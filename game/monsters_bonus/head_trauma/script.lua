function HeadTrauma_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_Discard(host, owner.id, 2)
    Common_BonusMonsterTail(host, me.id)
end