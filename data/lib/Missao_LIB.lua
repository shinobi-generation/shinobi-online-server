function Missao1(cid)
if (getPlayerStorageValue(cid, sto_missao[1]) < 1) then
	doPlayerAddItem(cid, 11879, 1)
	setPlayerStorageValue(cid, sto_missao[1], 1)
	selfSay('Obrigado e Boa Sorte na sua jornada', cid)
else
	selfSay('Desculpe, Mas voce ja realizou todas as missoes, nao tem mais missoes por enquanto.', cid)
end
end

function Missao2(cid)
if (getPlayerStorageValue(cid, sto_missao[1]) == 2) then
	selfSay('Obrigado e Boa Sorte na sua jornada', cid)
else
	selfSay('Desculpe, Mas voce ja realizou todas as missoes, nao tem mais missoes por enquanto.', cid)
end
end

function Missao3(cid)
if (getPlayerStorageValue(cid, sto_missao[1]) == 3) then
	selfSay('Obrigado e Boa Sorte na sua jornada', cid)
else
	selfSay('Desculpe, Mas voce ja realizou todas as missoes, nao tem mais missoes por enquanto.', cid)
end
end

function Missao4(cid)
if (getPlayerStorageValue(cid, sto_missao[1]) == 4) then
	selfSay('Obrigado e Boa Sorte na sua jornada', cid)
else
	selfSay('Desculpe, Mas voce ja realizou todas as missoes, nao tem mais missoes por enquanto.', cid)
end
end