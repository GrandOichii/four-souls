function Tony_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    local id = AttackLayers:top().id + 1
    AttackLayers:push(
        {
            id = id,
            func = function (host_, pid)
                local add = 0
                if pid == owner.id then
                    local o = Common_PlayerWithID(host, owner.id)
                    add = 2
                    for _, card in ipairs(o.board) do
                        if not card.isEternal and card.id ~= me.id then
                            add = add - 1
                            break
                        end
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

function Tony_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
end