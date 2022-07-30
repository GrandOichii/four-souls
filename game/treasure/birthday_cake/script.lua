function BirthdayCake_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function BirthdayCake_trigger(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        local choice = requestSimpleChoice(host, owner.id, 'Make '..player.name..' draw one card?', {'Yes', 'No'})
        if choice == 'Yes' then
            lootCards(host, player.id, 1)
        end
    end
end
