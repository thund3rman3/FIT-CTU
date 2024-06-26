Úkolem je realizovat třídu CPatchStr, která bude simulovat řetězec poskládaný z více částí (podřetězců).

Standardní typ std::string reprezentuje řetězec jako dynamicky alokované pole znaků. Velikost tohoto pole přizpůsobuje délce řetězce, nad řetězcem pak umí provádět základní operace (zřetězení, vložení řetězce do řetězce, vytvoření podřetězce, výmaz znaků). Podobnou funkcionalitu by měla nabízet požadovaná třída CPatchStr. Rozdíl je v tom, že naše třída si nebude pamatovat celý řetězec jako pole znaků, ale bude si pamatovat postup, kterým řetězec vznikl z primitivních komponent (string patch). Tento způsob ukládání pro dlouhé řetězce zrychlí výše zmíněné operace, navíc dokáže šetřit paměť.

Pro primitivní operaci (nastavení instance CPatchStr na zadaný C-řetězec) si samozřejmě řetězec budete muset zapamatovat jako jednotlivé znaky v nějakém poli. Předpokládejme ale jinou situaci. Mějme již nějak naplněné dvě instance CPatchStr v proměnných x a y. Chceme přidat obsah y za x, tedy provést x . append ( y ). Při standardním uložení by se paměťové nároky instance x zvětšily o velikost y a všechny znaky z y by se musely zkopírovat. Naše implementace si pouze zapamatuje vhodný na odkaz na instanci y, tím bude operace append zaznamenaná. Znaky se budou fyzicky kopírovat až v okamžiku, kdy bude opravdu potřeba znát skutečný obsah řetězce x. Pokud budou operace pro zřetězeni/mazání/vkládání časté, bude úspora velmi markantní.


Požadovaná třída CPatchStr má následující rozhraní:

implicitní konstruktor
vytvoří instanci třídy obsahující prázdný řetězec
konstruktor (const char *)
vytvoří instanci třídy, která bude inicializovaná zadaným řetězcem.
destruktor, operátor =, kopírující konstruktor
zajistí očekávané standardní chování. Automaticky generované varianty zřejmě nebudou stačit.
append ( x )
připojí řetězec x za konec existujícího řetězce. Návratová hodnota je reference na svojí instanci (this), volání lze tedy řetězit (fluent interface).
insert ( pos, x )
vloží řetězec x do existujícího řetězce od pozice pos. Pozice pos musí mít hodnotu od 0 do délky řetězce this (pro pos = len se metoda chová stejně jako append). Pokud je hodnota pos větší než délka řetězce this, vyhodí metoda výjimku std::out_of_range. Návratová hodnota je reference na svojí instanci (this), volání lze tedy řetězit.
remove ( from, len )
metoda odstraní znaky z existujícího řetězce. Odstraněné budou znaky od pozice from, celkem se odstraní len znaků. Mazané znaky musejí být z rozsahu řetězce this, tedy from + len musí být menší nebo rovné počtu znaků v řetězci this. Pokud tato podmínka není splněna, vyhodí metoda výjimku std::out_of_range. Hodnota len = 0 je platná, ale metoda pak nic nedělá. Návratová hodnota je reference na svojí instanci (this), volání lze tedy řetězit.
subStr ( from, len )
metoda vytvoří podřetězec řetězce this. Vzniklá instance CPatchStr bude reprezentovat len znaků řetězce this počínaje pozicí from. Podřetězec musí být z rozsahu řetězce this, tedy from + len musí být menší nebo rovné počtu znaků v řetězci this. Pokud tato podmínka není splněna, vyhodí metoda výjimku std::out_of_range. Hodnota len = 0 je platná, metoda vrací prázdný řetězec.
toStr ()
metoda sestaví řetězec podle zaznamenaných odkazů a provedených operací. Návratovou hodnotou je dynamicky alokovaný řetězec v konvenci ASCIIZ (nulou ukončený C-řetězec). Volající je zodpovědný za to, že alokovanou paměť uvolní až řetězec nebude dále potřebovat (zavolá delete []).
Odevzdávejte soubor, který obsahuje implementovanou třídu CPatchStr a další Vaše podpůrné třídy. Třída musí splňovat veřejné rozhraní podle ukázky - pokud Vámi odevzdané řešení nebude obsahovat popsané rozhraní, dojde k chybě při kompilaci. Do třídy si ale můžete doplnit další metody (veřejné nebo i privátní) a členské proměnné. Odevzdávaný soubor musí obsahovat jak deklaraci třídy (popis rozhraní) tak i definice metod, konstruktoru a destruktoru. Je jedno, zda jsou metody implementované inline nebo odděleně. Odevzdávaný soubor nesmí obsahovat vkládání hlavičkových souborů a funkci main. Funkce main a vkládání hlavičkových souborů může zůstat, ale pouze obalené direktivami podmíněného překladu jako v ukázce níže.

Úloha má procvičit práci s kopírováním objektů, počítanými referencemi a řetězci. Proto není k dispozici STL, zejména není k dispozici std::string, std::vector, std::list ani další kontejnery.

Nápověda:
Triviální řešení de facto reimplementuje std::string. V instanci se ukládá přímo sestavený řetězec. Metody insert/append/remove jsou jednoduché a pro krátké řetězce i časově efektivní. Pro dlouhé řetězce již řešení nebude vyhovovat. Pokud odevzdáte řešení tohoto typu, projde závaznými testy, ale neprojde testy nepovinnými (tedy bude hodnoceno méně než 100 % bodů).
Paměťově úspornější řešení si může pamatovat odkazy na základní řetězce (případně s dodatečnými informacemi o délce a posunutí). Tyto odkazy mohou být umístěné např. v poli. Operace insert/append/remove budou upravovat toto pole, případně upravovat délky a posunutí v něm zaznamenané. Metoda subStr pak do nové instance vloží výřez tohoto pole (opět s případně upravenými délkami a posunutími v odkazech).
Základní řetězce (které vzniknou kopií C-řetězců) je vhodné ukládat do instancí nějakých zapouzdřujících objektů. Tyto zapouzdřující objekty pak mohou implementovat nějakou techniku počítaných referencí. K dispozici je std::shared_ptr.
Testem paměťových nároků projde řešení, které pracuje na výše popsaném principu (pole odkazů na základní řetězce, počítané reference).
Test rychlosti 1 vytváří instance CPatchStr, které jsou tvořeny mnoha základními řetězci. Nad těmito instancemi je potřeba rychle vytvářet podřetězce, tedy je potřeba rychle vyhledat zadaný offset.
Test rychlosti 2 vytváří instance CPatchStr, které jsou tvořeny mnoha základními řetězci. Tyto instance pak často upravuje voláním append/insert/remove. Pro reprezentaci není vhodné pole, ale datová struktura, kde modifikace mají lepší než lineární časovou složitost.