function onTimer()
    local cor =  22
    local mensagens = {
[[DingDong: Bom Dia. Mais uma noite sem dormir, mas valeu a pena... Cheguei ao meu destino e cumpri mais uma miss�o.]],
[[DingDong: N�o conte cada hora do dia. Fa�a cada hora do seu dia valer a pena!]],
[[DingDong: Hoje eu s� quero que o dia termine bem!]]

}
	for i, pid in ipairs(getPlayersOnline()) do
		doSendPlayerExtendedOpcode(pid, 124,  "Default")
	end
	setGlobalStorageValue(pid, 910562, 2)
	doBroadcastMessage(mensagens[math.random(1,table.maxn(mensagens))], cor)
return true
end
