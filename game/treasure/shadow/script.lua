function Shadow_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    DeathStack:push({
        name = 'death'..me.id,
        func = function (host_, player)
            if player.id == owner.id then
                return DeathStack._et[DeathStack:posOf('death'..me.id)-1].func(host_, player)
            end
            local ownerID = player.id
            local function discard()
                local amount = PER_DEATH_LOOT
                amount = math.min(amount, #(player.hand))
                if amount ~= 0 then
                    local message = 'Choose a card to discard'
                    if amount > 1 then
                        message = 'Choose '..amount..' cards to discard'
                    end
                    local cardIDs = requestCardsInHand(host, ownerID, ownerID, message, amount)
                    for _, cid in ipairs(cardIDs) do
                        removeFromEverywhere(host, cid)
                        moveToHand(host, owner.id, cid)
                    end
                end
            end
            local function payCoins()
                local amount = math.min(PER_DEATH_COINS, player.coins)
                subCoins(host, player.id, amount)
            end
            local function destroyItems()
                local cardIDs = {}
                for _, card in ipairs(player.board) do
                    if not card.isEternal then
                        cardIDs[#cardIDs+1] = card['id']
                    end
                end
                if #cardIDs == 1 then
                    destroyCard(host, cardIDs[1])
                    return
                end
                if #cardIDs == 0 then
                    return
                end
                local choice, payed = requestChoice(host, owner.id, 'Choose a card to destroy', CARD, cardIDs)
                destroyCard(host, choice)
            end
            local function tapCards()
                tapCharacterCard(host, player.id)
                for _, card in ipairs(player.board) do
                    if not card.passive then
                        tapCard(host, card.id)
                    end
                end
            end
            discard()
            payCoins()
            destroyItems()
            tapCards()
            return true
        end
    })
end

function Shadow_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    DeathStack:remove('death'..me.id)
end
