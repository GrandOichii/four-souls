--  TODO untested

function Euthanasia_trigger_check(host, me)
    local turnC = getTurnCounter(host)
    local roll = Common_LastRoll(host, me)
    local owner = getOwner(host, me.id)
    -- check if is combat roll
    if not roll.isCombatRoll then
        CardData[me.id] = {
            lastRoll = -1,
            turnC = turnC
        }
        return false
    end
    -- check that the owner owns the roll
    if roll.ownerID ~= owner.id then
        CardData[me.id] = {
            lastRoll = -1,
            turnC = turnC
        }
        return false
    end
    -- check if the data is set or it's the same turn
    if CardData[me.id] == nil or CardData[me.id].turnC ~= turnC then
        CardData[me.id] = {
            lastRoll = roll.value,
            turnC = turnC
        }
        return false
    end
    local lastRoll = CardData[me.id].lastRoll
    if lastRoll == roll.value then
        CardData[me.id] = {
            lastRoll = -1,
            turnC = turnC
        }
        return true
    end
    CardData[me.id].lastRoll = roll.value
    return false
end

function Euthanasia_trigger(host)
    print('TRIGGERED')
    local me = this(host)
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        if monster.isBeingAttacked then
            CardData[me.id] = {}
            killEntity(host, MONSTER, monster.id)
            return
        end
    end
    print('FAILED TO FIND ATTACKED MONSTER')
end