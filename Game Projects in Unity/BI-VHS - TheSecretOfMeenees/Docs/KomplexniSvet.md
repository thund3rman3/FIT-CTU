## Mechaniky

### Kouzelní

Hráč si osvojí sesílání kouzel díky nalezené magické rukavici(tm). Samotné sesílání se provádí propojovením run v radiálním menu a obrazec(Spell Shape) vzniký propojením ovlivní typ kouzla. Každé kouzlo, či Spell Shape, využije jeden ze 4 elementů, které ovlivňují efektivnost proti typům nepřátel, ale i konkrétní efekt co kouzlo má.
Kouzla jsou různorodá a pomocí nich se může hráč dostat i přes překážky v prostředí. Je pouze na hráči, jak tyto mechanikuy ve svém průchodu hrou využije.

Například kámen je vždy hmatatelný objekt, po kterém lze chodit, nebo je možné ho využít jako překážku.

**Nyní jsou ve hře implementovaná pouze dvě kouzla** 
1.  Projektil:
    Jednoduchá koule, se snadným sesláním. Letí rovně, dokud do něčeho nenarazí.
2.  Box: O trochu složitější kouzlo, které se ve světě manifestuje ve formě "kvádru".

**Další možná kouzla**

3) Štít: Toto kouzlo se obalí okolo sesilatele, se speciálním efektem dle elementu.
4) Výbuch: Po dramatické chvilce toto kouzlo vyšle vlnu elementu, v podobě zvětšující se koule.

Zkrátka akční gameplay, při kterém si (časem) budete připadat jako skutečný čaroděj.

### Dungeon a combat

Dungeon je náhodně generovaný a s každým úspěšným průchodem se jeho komplexita a obtížnost navyšuje. Hráč musí zabít nepřátele v místnosti pomocí svých nově nabytých magických schopností, aby mohl pokročit dále - ale ne před zachráněním NPC.

### Smrt a Životy

Smrt vždy vede k zmrtvýchvstání na půdě vesnice. Životů je 100 v základu, Magmis Zala může počet životů zvětšit.

### Level-up systém

Level upování je mnohotvaré. V dungeonu se s každým dalším zvednutým Meenee zvedne síla kouzel spjatá s jeho živlem.

**Neimplementováno**
Pobně jako Magmis může hráči zvýšit životy, i jiná NPC poskytují podobné benefity.

## NPC
Old Man Leede: Starý starosta vesnice. Je zkrátka prastarý, stejně starý jako místo ve kterém se hráč nyní ocitl. Figuruje jako průvodce pro hráče, a otec pro ostatní NPC ve vesnici. Jeho role v příběhu je záměrně ponechána co nejvíce zastřená tajemstvím.

Aquid Monopole: Vodní shopkeeper. Jeho osobnost se zdánlivě točí kolem hledání zisku, avšak pod jeho hladinou se nacházejí i další hloubky.

**Neimplementováno**

Bob the Builder: Bob.

Magmis Zala: Kuchař vesnice, je první kdo hledá nové a čerstvé ingredience. NPC nepotřebují jíst - to však neznamená, že nechtějí!

Hurricane Tzu: Knihovník vesnice. Je sečtělý a rád předstírá, že toho mnoho ví. Je pro něj důležité vypravit se do hlubin pro získání nových vědomostí.

## Dialogy:
Implementované dialogy nabízejí možnosti volby, které jsou často povrchní. 

**Neimplementováno**
Některé dialogy jsou vyčerpatelné a objeví se pouze jednou, zatímco jiné se odemnknou až po dosažení určitých milestonů ve světě.
Například starosta vesnice, Old man Leede, má dialogy které se postupně otevírají až s přivedením jiných NPC.

### Jednoduché dialogy
Generic Meenees se občas na hráče otočí, a pronesou jednu z možných hlášek daných jejich elementem.

### Komplexní dialogy:
Old man Leede rozvine několik možných konverzací, už po příchodu do vesnice.

## Příběh:
První úkol je dostat se skrz tutoriál. Zde hráč část po části sesbírá elementy a magickou rukavici, která mu umožní sesílat kouzla ve 4 možných elementech. 
Další úkol je prezentovaný hlavou vesnice, Old Man Leedem, který hlavní postavě doporučí se vydat do hlubin Crucible a zachránit ostatní Meenees. 
Okolo této dynamiky se točí celý gameplay loop, kde úkoly jsou o zachránění NPC z hlubin.

**Neimplementováno**
Hráč postupně odhaluje hlavní záhadu a to jak se z místa dostat. Kromě toho může postupně odhalit mnoho jiných menších záhad a tajemství, jako například co se stalo předchozímu člověku v hlubinách.
Spoiler: zemřel stářím. 
Celý gameplay loop vydávání se do hlubin dungeonu skončí objevením tajemství - tedy, že jediný způsob jak se dostat ven, je zbavit se starosty vesnice, Leeda. S jeho smrtí je však nyní možné v dungeonu umřít, což vede k napínavé poslední cestě skrz.
