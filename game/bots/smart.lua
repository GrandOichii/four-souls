local lastActivation = ""

-- local COIN_CARDS = {
--     'One Cent',
--     'Two Cents',
--     'Three Cents',
--     'Four Cents',
--     'A nickel'
-- }

function PlayMain(me, state)
    return "$PASS"
end

function PlayRespond(me, state)
    return "$PASS"
end

function Bot_PromptAction(me, state)
    if me["id"] == state["currentID"] and state["isMain"] then
        return PlayMain(me, state)
    end
    return PlayRespond(me, state)
    -- return "$PASS"
end

function Bot_PromptResponse(me, state, text, choiceType, choices)
    
end

function Bot_PromptSimpleResponse(me, state, text, choices)
    
end