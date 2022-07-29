--  TODO untested
--  TODO unfinished

function ShadowDeathFunc(host, player)
    
end

function Shadow_enter(host, me, owner)
    DeathStack:push({
        name = 'death'..me.id,
        func = function (host_, player)
            if player.id == owner.id then
                DeathStack._et[DeathStack:posOf('death'..me.id)-1].func(host_, player)
                return
            end
            local amount = PER_DEATH_LOOT
            amount = math.min(amount, #(player.hand))
            if amount ~= 0 then
                local message = 'Choose a card to discard'
                if amount > 1 then
                    message = 'Choose '..amount..' cards to discard'
                end
                local cardIDs = requestCardsInHand(host, player.id, player.id, message, amount)
                for _, cid in ipairs(cardIDs) do
                    --  TODO give them to the owner
                    discardLoot(host, player.id, cid)
                end
            end
            amount = math.min(PER_DEATH_COINS, player.coins)
            subCoins(host, player.id, amount)
            addCoins(host, owner.id, amount)
            if #player.board ~= 1 then
                local cardIDs = {}
                for _, card in ipairs(player.board) do
                    if not card.isEternal then
                        cardIDs[#cardIDs+1] = card.id
                        print('ADDING CARD '..card.id..'\t'..card.name)
                    end
                end
                print('CHOICES SIZE: '..#cardIDs)
                if #cardIDs == 0 then
                    return
                end
                if #cardIDs == 1 then
                    print('DESTROYING FIRST CARD')
                    destroyCard(host, cardIDs[1])
                    print('FIRST CARD DESTROYED')
                    return
                end
                print('REQUESTING CHOICE')
                local choice, payed = requestChoice(host, owner.id, 'Choose a card to destroy', CARD, cardIDs)
                print('CHOICE RECEIVED: '..choice)
                print('DESTROYING')
                destroyCard(host, choice)
                print('DESTROYED')
            end
        end
    })
end

function Shadow_leave(host, me, owner)
    DeathStack:remove('death'..me.id)
end
