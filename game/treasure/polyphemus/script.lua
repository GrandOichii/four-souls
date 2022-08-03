function Polyphemus_enter(host, me, owner)
    CardData[me.id] = {
        iaID = Common_IncAttack(owner.id, 2),
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function Polyphemus_leave(host, me, owner)
    Common_DecAttack(host, CardData[me.id].iaID, owner.id)
    print('REMOVING MONSTER ROLL LAYER: '..CardData[me.id].irID)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end