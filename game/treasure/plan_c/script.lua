function PlanC_cost(host, cardInfo)
    return Common_Tap(host)
end

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
    destroyCard(host, me.id)
    local choice = Common_ChooseMonster(host, owner.id)
    killEntity(host, MONSTER, choice.id)
    deferEOT(host, me.id, 'PlanC_eot', true)    
end