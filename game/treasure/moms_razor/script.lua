function MomsRazor_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 6
end

function MomsRazor_trigger(host, me)
    local owner = getTopOwner(host)
    local pid = Common_LastRoll(host)["ownerID"]
    local player = Common_PlayerWithID(host, pid)
    local choice = requestSimpleChoice(host, owner["id"], "Deal 1 damage to "..player["name"].."?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    dealDamage(host, PLAYER, owner["id"], PLAYER, pid, 1)
end