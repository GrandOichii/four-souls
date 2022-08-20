function HolyMomsEye_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 1)
end

function HolyMomsEye_trigger_check(host, me)
	return Common_LastRoll(host).value == 2
end

function HolyMomsEye_trigger(host)
	local ownerID = Common_LastRoll(host).ownerID
	local choice = requestSimpleChoice(host, ownerID, 'Recharge an item?', {'Yes', 'No'})
	if choice == 'No' then
		return
	end
	Common_RechargeAnItem(host, ownerID, true)
end
