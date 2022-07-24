--  TODO untested

function DemonForm_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_TempIncAttack(host, me.id, owner.id, 2)
end