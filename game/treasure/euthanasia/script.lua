--  TODO untested

function Euthanasia_trigger_check(host, me)
    local roll = Common_LastRoll(host)
    local owner = getOwner(host, me.id)
    if not roll.isCombatRoll then
        CardData[me.id] = {}
        return false
    end
    if roll.ownerID ~= owner.id then
        CardData[me.id] = {}
        return false
    end
    if CardData[me.id] == nil then
        CardData[me.id] = {
            lastRoll = roll.value
        }
        return false
    end
    local lastRoll = CardData[me.id].lastRoll
    if lastRoll == roll.value then
        CardData[me.id] = {}
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