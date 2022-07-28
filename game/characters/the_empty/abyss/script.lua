function Abyss_cost(host, cardInfo)
    return Common_Tap(host)
end

function Abyss_tap(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local ids = {}
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return
    end
    local choice, _ = requestChoice(host, owner.id, 'Choose an item to sacrifice', CARD, ids)
    destroyCard(host, choice)
    addCounters(host, me.id, 1)
end

function Abyss_enter(host, me, owner)
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
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local me_ = getMe()
                    -- print('ADD: '..(me.counters // 2))
                    add = me_.counters // 2
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
end