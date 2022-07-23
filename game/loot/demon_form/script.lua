--  TODO untested

function DemonForm_use(host)
    local owner = getTopOwner(host)
    tempIncAttack(host, owner.id, 2)
end