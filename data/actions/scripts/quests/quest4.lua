function onUse(cid, item, fromPosition, itemEx, toPosition)

local a = 9854151133

	if getPlayerStorageValue(cid, a) < 1 then
		doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "Voce completou a quest.")
		doPlayerAddItem(cid, 2148, 60)
		doPlayerAddItem(cid, 13281, 1)
		doPlayerAddExp(cid,20000)
		doPlayerSetStorageValue(cid, a, 1)
	else
		doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "It is empty.")
	end

	return true
end