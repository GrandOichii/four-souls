local deathEvent = getLastDeath()
if (deathEvent["entity"] ~= PLAYER) then
    os.exit()
end
local owner = getOwner()
lootCards(owner["id"], 1)