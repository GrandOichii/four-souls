function MomsShovel_enter(host)
    local me = this(host)
    tapCard(host, me.id)
end

function MomsShovel_effect(host)
    local owner = getTopOwner(host)
    local choice = Common_ChooseAnySoul(host, owner.id)
    removeFromEverywhere(host, choice)
    addSoulCard(host, owner.id, choice)
end
