function RageCreep_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function RageCreep_trigger_check(host, me)
	return Common_MonsterDealtCombatDamage(host, me.id)
end

function RageCreep_trigger(host)
    local me = this(host)
	local e = CardData[me.id].damageEvent
	local cPlayer = getCurrentPlayer(host)
    local players = getPlayers(host)
    local extraID = -1
    for i, player in ipairs(players) do
        if player.id == cPlayer.id then
            extraID = i + 1
            break
        end
    end
    if extraID == -1 then
        print('LUA ERROR: CAN\'T FIND PLAYER WITH ID: '..cPlayer.id)
        os.exit(1)
    end
    if extraID == #players + 1 then
        extraID = 1
    end
    extraID = players[extraID].id
    dealDamage(host, e.sourceType, e.sourceID, PLAYER, extraID, e.amount)
end
