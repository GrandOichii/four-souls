TURN_COUNTER = 0

function Bot_PromptAction(me, state)
    if me["id"] == state["currentID"] and state["isMain"] then
        -- main phase actions
        TURN_COUNTER = TURN_COUNTER + 1
        print("Player" .. me["name"] .. " turn counter: " .. TURN_COUNTER)
    end
    return "$PASS"
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    
end