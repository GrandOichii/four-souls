--  TODO untested

function GBG_trigger_check(host, cardInfo)
    return Common_LastRoll(host).value == 1
end

function GBG_trigger(host)
    local owner = getTopOwner(host)
    local pid = Common_LastRoll(host).id
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
    local choice, _ = requestChoice(host, owner.id, 'Choose an item', CARD, ids)
    Common_RerollItem(host, choice.id)
end