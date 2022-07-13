-- This bot always plays a random loot card, buys a random treasure (if can), then passes the turn
-- Targets are chosen randomly

-- local function ()
    
-- end

function Bot_PromptAction(me, state)
    local lootCards = me["hand"]
    if #lootCards ~= 0 then
        if me["playableCount"] ~= 0 then
            return "play_loot " .. lootCards[1]["id"]    
        end
        if me["characterActive"] then
            return "activate_character"
        end
    end
    if me["id"] == state["currentID"] and state["isMain"] then
        -- main phase actions
        if me["purchaseCount"] ~= 0 and me["coins"] >= me["treasurePrice"] then
            return "buy_treasure 0"
        end
    end
    -- responses
    local trinkets = me["board"]
    for _, trinket in pairs(trinkets) do
        if not trinket["passive"] and not trinket["tapped"] then
            return "activate " .. trinket["id"] .. " 0"
        end
    end
    return "$PASS"
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    
end

function Bot_PromptSimpleResponse(me, state, text, choices)
    
end