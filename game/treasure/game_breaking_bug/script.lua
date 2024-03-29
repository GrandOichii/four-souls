function GBG_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 1
end

function GBG_trigger(host)
    local owner = getTopOwner(host)
    local pid = CardData[this(host).id].lastRoll.ownerID
    local target = Common_PlayerWithID(host, pid)
    local choice = requestSimpleChoice(host, owner.id, 'Reroll one of '..target.name..'\' items?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local ids = {}
    for _, item in ipairs(target.board) do
        if not item.isEternal then
            ids[#ids+1] = item.id
        end
    end
    if #ids == 0 then
        return
    end
    local choice, _ = requestChoice(host, owner.id, 'Choose an item', CARD, ids)
    Common_RerollItem(host, choice)
end