function DemonForm_use(host)
    discardMe(host, this(host).id)

    local me = this(host)
    local owner = getTopOwner(host)
    Common_TempIncAttack(host, me.id, owner.id, 2)

end