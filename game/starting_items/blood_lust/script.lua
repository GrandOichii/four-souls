function BloodLust_tapEOT(host)
    print("the effect wore off")
end

function BloodLust_tap(host)
    print("blood lust was tapped")

    deferEOT(host, this(host)["id"], "BloodLust_tapEOT", false)
end