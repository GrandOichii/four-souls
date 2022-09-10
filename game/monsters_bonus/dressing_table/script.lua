function DressingTable_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_RerollAny(host, owner.id)
    Common_BonusMonsterTail(host, me.id)
end