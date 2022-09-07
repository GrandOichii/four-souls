function ForgetMeNow_effect(host)
    local me = this(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local owner = getOwner(host, me.id)
    if owner == nil then
        return
    end
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        local choice, chosen = Common_ChooseSoul(host, player.id, player.id)
        if chosen then
            destroySoul(host, player.id, choice)
        end
    end
end
