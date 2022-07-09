local rollEvent = getLastRoll()
if (rollEvent["amount"] ~= 5) then
    return
end
local owner = getOwner()
addCoins(owner["id"], 3)