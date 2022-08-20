function TheButterBean_tap(host)
    local card = this(host)
    local roll = popRollStack(host)
    if roll == 6 then
        rechargeCard(host, card.id)
    end
end