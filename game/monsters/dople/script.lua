function Dople_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 7)
end

function Dople_trigger_check(host, me)
	return Common_MonsterDamaged(host, me.id)
end

function Dople_trigger(host)
    local me = this(host)
	local e = CardData[me.id].damageEvent
    local cPlayer = getCurrentPlayer(host)
    local players = getPlayers(host)
    local extraID = -1
    for i, player in ipairs(players) do
        if player.id == cPlayer.id then
            extraID = i - 1
            break
        end
    end
    if extraID == -1 then
        print('LUA ERROR: CAN\'T FIND PLAYER WITH ID: '..cPlayer.id)
        os.exit(1)
    end
    if extraID == 0 then
        extraID = #players
    end
    extraID = players[extraID].id
    dealDamage(host, e.sourceType, e.sourceID, PLAYER, extraID, e.amount)
end
