

function Hypercoagulation_enter(host, me, owner)
    local meID = me.id
    local function getMe()
        local p = Common_PlayerWithID(host, owner.id)
        for _, card in ipairs(p.board) do
            if card.id == meID then
                return card
            end
        end
        -- return nil
    end
    local id = MaxHealthLayers:top().id + 1
    MaxHealthLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local me_ = getMe()
                    add = me_.counters
                end
                return add + MaxHealthLayers._et[MaxHealthLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
end

function Hypercoagulation_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function Hypercoagulation_trigger1(host)
    local me = this(host)
    local owner = getTopOwner(host)
    addCounters(host, me.id, 1)
    healPlayer(host, owner.id, 1)
end

function Hypercoagulation_trigger2_check(host, me)
    local isTrue = Common_OwnersTurn(host, me.id)
    if not isTrue then
        return false
    end
    return me.counters >= 3
end

function Hypercoagulation_trigger2(host)
    local me = this(host)
    removeCounters(host, me.id, me.counters)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end
