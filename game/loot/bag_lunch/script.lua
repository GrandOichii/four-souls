local function BL_IncLife(host, me, owner)
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
    CardData[me.id] = {
        ihID = id
    }
    MaxHealthLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local me_ = getMe()
                    if me_.counters > 0 then
                        add = 1
                    end
                end
                return add + MaxHealthLayers._et[MaxHealthLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
end

function BL_ModDestroyStack(host, me, owner)
    local function getMe()
        local p = Common_PlayerWithID(host, owner.id)
        for _, card in ipairs(p.board) do
            if card.id == me.id then
                return card
            end
        end
        -- return nil
    end
    DestroyFuncStack:push({
        name = 'destroy'..me.id,
        func = function (host_, cardID)
            local me_ = getMe()
            print('ME: id - '..me_.id..'  COUNTERS: '..me_.counters)
            if cardID ~= me.id or me_.counters > 0 then
                DestroyFuncStack._et[DestroyFuncStack:posOf('destroy'..me.id)-1].func(host_, cardID)
                return
            end
            addCounters(host_, me_.id, 1)
            healPlayer(host_, owner.id, 1)
        end
    })

end

function BagLunch_enter(host, me, owner)
    BL_ModDestroyStack(host, me, owner)
    BL_IncLife(host, me, owner)
end

function BagLunch_leave(host, me, owner)
    DestroyFuncStack:remove('destroy'..me.id)
    Common_DecMaxLife(host, CardData[me.id].ihID, owner.id)
end