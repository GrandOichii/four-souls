function PlanC_eot(host)
    --  TODO may not work
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        killEntity(host, PLAYER, player.id)
    end
end

function PlanC_tap(host, cardInfo)
    local owner = getTopOwner(host)
    local me = this(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    local choice = Common_ChooseMonster(host, owner.id)
    killEntity(host, MONSTER, choice.id)
    deferEOT(host, me.id, 'PlanC_eot', true)
end