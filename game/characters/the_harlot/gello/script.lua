function Gello_cost(host, info)
    local ownerID = info.ownerID
    local monsters = getActiveMonsters(host)
    local ids = {}
    for _, monster in ipairs(monsters) do
        if monster.isBeingAttacked then
            ids[#ids+1] = monster.id
        end
    end
    if #ids == 0 then
        return false
    end
    local tid = ids[1]
    if #ids ~= 1 then
        tid, _ = requestChoice(host, ownerID, 'Choose a monster being attacked', MONSTER, ids)
    end
    pushTarget(host, tid, MONSTER)
    ids = {}
    for _, monster in ipairs(monsters) do
        if monster.id ~= tid then
            ids[#ids+1] = monster.id
        end
    end
    local nID, _ = requestChoice(host, ownerID, 'Choose a monster to deal damage to', MONSTER, ids)
    pushTarget(host, nID, MONSTER)
    return true
end

function Gello_effect(host)
    local owner = getTopOwner(host)
    local dTarget = popTarget(host)
    local hTarget = popTarget(host)
    local dMonster, found1 = Common_MonsterWithID(host, dTarget.id)
    if not found1 then
        return
    end
    local hMonster, found2 = Common_MonsterWithID(host, hTarget.id)
    if not found2 then
        return
    end
    local oldH = hMonster.health
    healMonster(host, hMonster.id, oldH)
    hMonster, found2 = Common_MonsterWithID(host, hTarget.id)
    if not found2 then
        return
    end
    local newH = hMonster.health
    local diff = newH - oldH
    print('HEALED: '..diff)
    if diff <= 0 then
        return
    end
    dealDamage(host, PLAYER, owner.id, MONSTER, dMonster.id, diff)
    local cPlayer = getCurrentPlayer(host)
    if owner.id ~= cPlayer.id then
        cancelCurrentAttack(host)
        Common_IncAttackCount(host, cPlayer.id, false)
    end
end