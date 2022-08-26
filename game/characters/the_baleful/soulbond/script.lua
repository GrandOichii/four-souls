function Soulbond_enter(host)
    local owner = getTopOwner(host)
	local me = this(host)
    DeathStack:push({
        name = 'death'..me.id,
        func = function (host_, player)
            if player.id ~= owner.id then
                DeathStack._et[DeathStack:posOf('death'..me.id)-1].func(host_, player)
                return
            end
            local ownerID = player.id
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
                local choice, payed = requestChoice(host, ownerID, 'Choose a card to destroy', CARD, cardIDs)
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
            destroyItems()
            tapCards()
        end
    })
end

function Soulbond_leave(host)
    local me = this(host)
    DeathStack:remove('death'..me.id)
end

function Soulbond_trigger_check(host, me)
    return Common_OwnerDied(host, me.ownerID)
end

function Soulbond_trigger_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function Soulbond_trigger_effect(host)
    local target = popTarget(host)
    killEntity(host, PLAYER, target.id)
end
