function MomsShovel_enter(host)
    local me = this(host)
    tapCard(host, me.id)
end

function MomsShovel_effect(host)
    local owner = getTopOwner(host)
    local choice, _, chosen = Common_ChooseAnySoul(host, owner.id)
    if chosen then
        removeFromEverywhere(host, choice)
        addSoulCard(host, owner.id, choice)
    end
end
