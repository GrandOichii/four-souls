function RibOfGreed_enter(host)
    local me = this(host)

    local dllID = DiscardLootLayers:top().id + 1
    DiscardLootLayers:push(
        {
            id = dllID,
            func = function (host_, ownerID, cid, isCost)
                if not isCost then
                    return DiscardLootLayers._et[DiscardLootLayers:posOf(dllID)-1].func(host_, ownerID, cid, isCost)
                end
            end
        }
    )

    local cdlID = Common_DiscardLayers:top().id + 1
    Common_DiscardLayers:push(
        {
            id = cdlID,
            func = function (host_, ownerID, amount, isCost)
                if not isCost then
                    return Common_DiscardLayers._et[Common_DiscardLayers:posOf(cdlID)-1].func(host_, ownerID, amount, isCost)
                end
            end
        }
    )


    CardData[me.id] = {
        dllID = dllID,
        cdlID = cdlID
    }
end

function RibOfGreed_leave(host)

end
