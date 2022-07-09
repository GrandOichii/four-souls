local owner = getOwner()
local choice = requestChoice(owner["id"], "Recharge Sack of Pennies?", "Yes|No")
if (choice == "No") then
    os.exit()
end
local id = this()["id"]
rechargeCard(id)