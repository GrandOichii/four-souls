TURN_COUNTER = 0

function Bot_PromptAction(me, state)
    if me["id"] == state["currentID"] and state["isMain"] then
        -- main phase actions
        local lootCards = me["hand"]
        print("My cards:" .. #lootCards)
        print(me["playableCount"])
        if me["playableCount"] == 0 then
            return "$PASS"
        end
        if #lootCards == 0 then
            return "$PASS"
        end
        -- play first card
        local card = lootCards[1]
        return "play_loot " .. card["id"]
    end
    return "$PASS"
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    
end