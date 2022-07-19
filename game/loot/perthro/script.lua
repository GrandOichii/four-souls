function Perthro_use(host)
    local owner = getTopOwner(host)
    local choice, chosen = Common_ChooseNonEternalCard(host, owner.id)
    if not chosen then
        return
    end
    Common_RerollItem(host, choice)
end