function Jester_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_RerollAny(host, owner.id)
    discardMe(host, me.id)
end