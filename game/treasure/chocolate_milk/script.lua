function ChocolateMilk_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = AttackLayers:top().id + 1
    local function getMe()
        local p = Common_PlayerWithID(host, owner.id)
        for _, card in ipairs(p.board) do
            if card.id == me.id then
                return card
            end
        end
    end
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local me_ = getMe()
                    if me_.counters == 2 then
                        add = 1
                    end
                end
                return add + AttackLayers._et[AttackLayers:posOf(id)-1].func(host_, pid)
            end
        }
    )
    CardData[me.id] = {
        iaID = id
    }
end

function ChocolateMilk_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end

function ChocolateMilk_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function ChocolateMilk_trigger(host)
    local me = this(host)
    local owner = getTopOwner(host)
    addCounters(host, me.id, 1)
    me = this(host)
    if me.counters >= 3 then
        removeCounters(host, me.id, me.counters)
        local tgtType = nil
        local tgtID = nil
        local choice = requestSimpleChoice(host, owner.id, 'Deal 1 damage to what', {'Monster', 'Player'})
        if choice == 'Monster' then
            tgtType = MONSTER
            tgtID = Common_ChooseMonster(host, owner.id).id
        else
            tgtType = PLAYER
            tgtID = Common_ChoosePlayer(host, owner.id)
        end
        dealDamage(host, PLAYER, owner.id, tgtType, tgtID, 1)
    end
end
