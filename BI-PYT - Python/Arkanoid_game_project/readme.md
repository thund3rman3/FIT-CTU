ARKANOID - README

Je potøeba si stáhnou modul pygame - Pygame verze 2.1
Na windows se stahne pomocí: py -m pip install -U pygame –user
Dále jsem použil modul randint.

Používal jsem Python 3.9 a conda environment interpereter.

Hra se zapne spustìním main.py

Poznámky:
	-Za nièení kostek získáte skóre
	-Po všech znièení kostek v jednom levelu nemùže umøít, dokud nejdete do dalšího
	-Stejný level se opakuje poøád dokola, pøièemž se vám navyšuje skóre

Ovládání:
A – pohyb doleva
D – pohyb doprava
W – vystøelení laseru, podkud nìjaké máte
	- náhodnì se vám pøidají pøi nièení kostek
	- poèet støel indikuje in-game UI napravo
SPACE – pauznutí  a odpauznutí hry
ENTER ( pygame tomu øíká taky RETURN) 
	- pøechod scén
	-hra vám øekne až to budete muset zmáèknout
