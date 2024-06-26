# FITExcel

Úkolem je realizovat třídu (sadu tříd), která bude fungovat jako tabulkový procesor. Tabulkový procesor bude umožňovat operace s buňkami (nastavení, výpočet hodnoty, kopírování), bude umět vypočítávat hodnotu buňky podle vzorce, dokáže detekovat cyklické závislosti mezi buňkami a dokáže obsah tabulky uložit a načíst. Hodnocení se kromě funkčnosti zaměří na vhodný návrh tříd, použití polymorfismu a vhodné využití verzovacího systému.

Funkcionalita je rozdělena na několik částí, hodnocení bude záviset na tom, kolik požadavků dokáže vaše implementace pokrýt. Dále, Vaše implementace nebude muset realizovat vlastní syntaktickou analýzu výrazů, které se v buňkách budou objevovat. Syntaktický analyzátor bude dostupný v testovacím prostředí a je přiložen v dodaném archivu v podobě staticky linkované knihovny.

Rozhraní počítá s následujícími třídami:
CSpreadsheet - vlastní tabulkový procesor, který implementujete. Požadované veřejné rozhraní je popsáno níže, jeho implementace je na Vás.
CPos - identifikátor buňky v tabulce. Buňky v tabulce identifikujeme standardním způsobem: neprázdná sekvence písmen A až Z udává sloupec a následuje nezáporné číslo řádky (0 považujeme za platné číslo řádky). Písmena v názvu sloupce mohou být malá i velká (case insensitive). Sloupce jsou značené postupně A, B, C, ..., Z, AA, AB, AC, ..., AZ, BA, BB, ..., ZZZ, AAAA, ... Příklady identifikátorů buněk: A7, PROG7250, ... Pokud si nejste jisti značením, podívejte se na skutečný tabulkový procesor. Takto zadaný identifikátor buňky může být nešikovný pro implementaci, proto je zapouzdřen do třídy CPos, která si jej může převést do vhodnější reprezentace. Implementace převodu a třídy CPos je na Vás.
CExpressionBuilder - abstraktní třída, kterou používá dodaný analyzátor výrazů. Pokud se rozhodnete používat dodaný analyzátor, budete muset implementovat podtřídu a její instanci předat analyzátoru.
parseExpression() - funkce v testovacím prostředí, která provede syntaktickou analýzu zadaného výrazu a identifikované části tohoto výrazu předá ke zpracování Vaší podtřídě CExpressionBuilder.
CValue - reprezentuje hodnotu buňky. Hodnota buňky je buď nedefinovaná, desetinné číslo, nebo řetězec. Třída CValue je pojmenovaná specializace generické třídy std::variant.
další - pro implementaci si zřejmě budete muset vytvořit mnoho svých vlastních tříd a funkcí.
Výrazy v buňkách:
Implementace požaduje, aby buňky tabulky mohly být prázdné (nedefinované), mohly obsahovat číslo, řetězec, nebo vzorec pro výpočet hodnoty. Vzorce mají syntaxi podobnou standardnímu tabulkovému procesoru, jsou ale omezené počtem implementovaných funkcí. Výraz může obsahovat:

číselný literál - celé nebo desetinné číslo, volitelně s exponentem, tedy např. 15, 2.54, 1e+8, 1.23e-10, ...
řetězcový literál - řetězec je posloupnost znaků zapsaná v uvozovkách. Pokud má řetězec obsahovat uvozovku, je tato uvozovka zdvojena. Řetězcový literál může obsahovat libovolné znaky včetně znaku pro odřádkování.
odkaz na buňku - odkaz je realizován ve standardní notaci (sloupec - posloupnost písmen, řádka - číslo). Malá a velká písmena se v odkazu na sloupec nerozlišují. Příklad odkazů: A7, ZXCV789456, ... Ve vzorci lze odkazovat na buňku absolutně nebo relativně. Navíc lze absolutní / relativní odkaz zadat nezávisle pro řádek a sloupec. Absolutní odkaz znamená, že odkazovaná buňka bude stále stejná, i když výraz v buňce zkopírujeme do jiné buňky. Relativní odkaz znamená, že při kopírování vzorce do jiné buňky se odkazovaná buňka změní. Absolutní odkaz je uvozen znakem $. Příklady: A5, $A5, $A$5, A$5.
rozmezí buněk (range) - výraz definuje obdélníkové pole buněk, pozice jsou dané levým horním a pravým dolním rohem, oddělovačem je znak dvojtečka. Příklad: A5:X17. Odkazy na buňky opět mohou být absolutní nebo relativní, absolutnost/relativnost odkazu se uplatní při kopírování. Příklady: A$7:$X29, A7:$X$29, ...
funkce - výraz v buňce může volat funkce a předat jim parametry. Implementace požaduje následující funkce:
sum (range) - v daném rozmezí sečte hodnoty všech buněk, která se vyhodnotí jako číslo. Pokud v rozmezí neexistuje žádná buňka s číselnou hodnotou, bude výsledkem funkce sum nedefinovaná hodnota.
count (range) - v daném rozmezí spočte všechny buňky, které mají definovanou hodnotu.
min (range) - v daném rozmezí projde hodnoty všech buněk, která se vyhodnotí jako číslo. Výsledkem je hodnota nejmenšího nalezeného čísla. Pokud v rozmezí neexistuje žádná buňka s číselnou hodnotou, bude výsledkem funkce min nedefinovaná hodnota.
max (range) - v daném rozmezí projde hodnoty všech buněk, která se vyhodnotí jako číslo. Výsledkem je hodnota největšího nalezeného čísla. Pokud v rozmezí neexistuje žádná buňka s číselnou hodnotou, bude výsledkem funkce max nedefinovaná hodnota.
countval (value, range) - v daném rozmezí projde hodnoty všech buněk a spočítá ty z nich, které mají hodnotu stejnou jako vyhodnocený výraz value. Porovnání hodnot typu řetězec je jasně dané, při porovnávání čísel uvažujte přesnou shodu (bez epsilon tolerance),
if (cond, ifTrue, ifFalse) - podmínka vyhodnotí výraz podmínky cond. Pokud je hodnota podmínky nedefinovaná nebo řetězec, je výsledkem funkce if nedefinovaná hodnota. Pokud je výsledkem podmínky nenulové číslo je výsledkem if hodnota výrazu ifTrue, pokud je výsledkem podmínky číslo 0, je výsledkem if hodnota výrazu ifFalse.
kulaté závorky - umožňují změnit prioritu vyhodnocení podvýrazu.
binární operátory ^, *, / a - slouží pro umocňování, násobení, dělení a odčítání. Všechny vyžadují dva číselné operandy. Pokud je alespoň jeden operand jiného typu, je výsledkem nedefinovaná hodnota. Dále, dělení nulou se vyhodnotí jako nedefinovaná hodnota, hodnotu 0 dělence porovnávejte přímo operátory jazyka (neuvažujte epsilon toleranci),
operátor + pracuje pro čísla podobně jako ostatní binární operátory. Navíc je schopen zřetězovat operandy v kombinaci číslo+řetězec, řetězec+číslo a řetězec+řetězec. Pro převod čísla na řetězec použijte funkci std::to_string.
unární operátor - slouží ke změně znaménka. Je použitelný pouze pro číselný operand, jinak se vyhodnotí na nedefinovanou hodnotu.
relační operátory < <=, >, >=, <>, = slouží pro porovnávání operandů. Operandy musí být dvě číselné hodnoty nebo dvě řetězcové hodnoty. Výsledkem je číslo 1 (podmínka platí) nebo 0 (podmínka neplatí). Pokud je alespoň jeden z operandů nedefinovaná hodnota nebo pokud se porovnávají hodnoty číslo-řetězec nebo řetězec-číslo, je výsledek porovnání nedefinovaná hodnota. Desetinná čísla porovnávejte přímo operátory jazyka (bez epsilon tolerance).
Priorita operátorů a asociativitu operátorů, v pořadí od nejvyšší priority:
volání funkce,
umocnění ^, levá asociativita,
unární -, pravá asociativita,
násobení * a dělení /, levá asociativita,
sčítání/zřetězení + a odčítání -, levá asociativita,
relační operátory < <=, > a >=, levá asociativita,
operátory rovnost/nerovnost <> a =, levá asociativita.
Výše uvedená pravidla jsou důležitá zejména pokud se rozhodnete pro implementaci vlastního parseru. Dodaná funkce parseExpression() zpracovává vstup podle výše popsaného postupu a používá popsané priority a asociativity.

Třída CPos
třída reprezentuje identifikátor buňky. Testovací prostředí používá pouze konstruktor této třídy, kterému předá řetězec s názvem buňky (např. "A7"). Implementace tento řetězec zpracuje, zkontroluje a uloží do členských proměnných. Pokud konstruktor dostane neplatný identifikátor buňky, vyhodí výjimku std::invalid_argument. Vnitřní realizace třídy a její další rozhraní je na Vás.

Funkce parseExpression() a třída CExpressionBuilder
Funkce parseExpression( expr, builder ) představuje rozhraní dodaného analyzátoru výrazů, které lze použít v buňkách. Parametrem volání je řetězec s výrazem expr a instance builder, jejíž rozhraní bude voláno během analýzy výrazu. Funkce nevrací žádnou hodnotu, zpracovaný výraz si bude v sobě ukládat dodaná instance builder. Funkce výraz buď zpracuje bez chyb, nebo vyhodí výjimku std::invalid_argument s popisem chyby. Pro správné použití funkce je nutné dodat vlastní builder. Ten vznikne jako potomek abstraktní třídy CExpressionBuilder, realizace této podtřídy je Váš úkol.

Syntaktická analýza výrazu vytvoří z infixového zápisu (např. 4 + A2 + B$7) jeho postfixovou podobu, tedy pro ukázkový příklad to bude: 4 A2 + B$7 +. Tato postfixová podoba se nevytváří explicitně. Parser místo toho volá odpovídající metody builderu, pro ukázku by volal:

valNumber (4)
valReference ("A2")
opAdd ()
valReference ("B$7")
opAdd ()
Postfixová podoba je mnohem jednodušší na zpracování. Dodávané hodnoty operandů (valNumber, valString, valReference, ...) si ukládejte na zásobník ve své instanci podtřídy CExpressionBuilderu. Přicházející operátor (opAdd, opSub, ..., funcCall) znamená, že si vyzvednete odpovídající počet operandů ze svého zásobníku a tyto operandy spojíte odpovídajícím operátorem (např. operátorem +). Musíte dávat pozor na pořadí operandů, levější operandy jsou na zásobníku hlouběji. Výsledek si uložíte na zásobník. Po úspěšném zpracování celého výrazu budete mít na zásobníku jedinou položku - vyhodnocení celého výrazu.

Rozhraní dodaného parseru je flexibilní. Umožní například vyhodnocovat výraz rovnou během syntaktické analýzy - na zásobník budete ukládat hodnoty vyhodnocených podvýrazů. Tedy opakovaným zpracování výrazu v buňce lze vypočítat novou hodnotu výrazu (např. po změně obsahu nějaké buňky, na které hodnota výrazu záleží). Takový postup je ale velmi pomalý. Proto využijte dodaný parser efektivněji - použijte jej pro konstrukci AST. Opakované vyhodnocení AST je režijně mnohem méně náročné než opakované parsování celého výrazu.

Abstraktní třída CExpressionBuilder
opAdd()
aplikace operátoru sčítání/zřetězení (+),
opSub()
aplikace operátoru odčítání (-),
opMul()
aplikace operátoru násobení (*),
opDiv()
aplikace operátoru dělení (/),
opPow()
aplikace operátoru umocnění (^),
opNeg()
aplikace operátoru unární mínus (-),
opEq()
aplikace operátoru porovnání na rovnost (=),
opNe()
aplikace operátoru porovnání na nerovnost (<>),
opLt()
aplikace operátoru porovnání menší než (<),
opLe()
aplikace operátoru porovnání menší nebo rovno (<=),
opGt()
aplikace operátoru porovnání větší než (>),
opGe()
aplikace operátoru porovnání větší nebo rovno (>=),
valNumber ( num )
přidání hodnoty číselného literálu, parametrem je hodnota čísla,
valString ( str )
přidání hodnoty řetězcového literálu, parametrem je hodnota řetězce,
valReference ( str )
přidání odkazu na jinou buňku, parametrem je identifikátor buňky (např. A7 nebo $x$12),
valRange ( str )
přidání odkazu na rozmezí buněk (range), parametrem je identifikace obdélníkového výřezu buněk (např. A7:$C9 nebo $x$12:z29),
funcCall ( fnName, paramCnt )
přidání volání funkce, parametrem je jméno funkce fnName a počet parametrů funkce paramCnt. Dodaný parser kontroluje, zda je zadané jméno funkce platné (sum, min, max, count, if nebo countval) a dále kontroluje, že počty a typy parametrů souhlasí (např. že if má 3 parametry a žádný není range, sum musí mít jeden parametr typu range, ...). Tedy tuto kontrolu nemusíte na straně builderu provádět. Kontrola je samozřejmě potřeba, pokud se rozhodnete pro vlastní parser.
Třída CSpreadsheet
implicitní konstruktor
Vytvoří prázdnou tabulku.
kopírující konstruktor/přesouvací konstruktor/operátor =, destruktor
Zajistí správné kopírovací operace s instancí.
save ( os )
Metoda uloží obsah instance do zadaného výstupního streamu. Z takto uložených dat musí být možné zrekonstruovat původní obsah instance. Metoda vrací true pro úspěch, false pro chybu při zápisu.
load ( is )
Metoda načte obsah instance CSpreadsheet ze zadaného vstupního streamu. Vrací true pro úspěch, false pro chybu (poškozený obsah).
setCell ( pos, contents )
Metoda nahradí obsah zadané buňky pos obsahem content. Obsah je zadán jako řetězec, ten může obsahovat:
platné desetinné číslo, tedy např. setCell ( CPos ("A1"), "123456.789e-9" ),
řetězec, tedy libovolnou posloupnost znaků, která nezačíná znakem =. Například setCell ( CPos ("A1"), "abc\ndef\\\"" ),
výraz, který se použije pro výpočet hodnoty buňky. Výraz začíná znakem =, například: setCell ( CPos ("A1"), "=A5-($B7*c$4+sum($X9:AC$124))^2" ),
Návratovou hodnotou metody setCell je true (úspěch), nebo false (neúspěch pro neplatný zadaný výraz, buňka zůstane beze změn). Není úplně rozumné ihned po zavolání setCell přepočítat hodnoty všech buněk, které na změněné hodnotě buňky závisí. Časově výhodnější je hodnotu buňky vypočítat až při následném volání getValue.
getValue ( pos )
Metoda vypočte obsah buňky a vrátí jej jako návratovou hodnotu. Pokud hodnotu nelze vypočítat (obsah buňky není definovaný, výpočet obsahuje cyklus), vrátí metoda hodnotu CValue().
copyRect ( dstCell, srcCell, w, h )
Metoda zkopíruje čtverec buněk o velikosti w sloupců a h řádek z pozice srcCell (levý horní roh zdrojového čtverce) na pozici dstCell (levý horní roh cílového čtverce). Pozor, zdrojový a cílový čtverec se mohou překrývat.
capabilities ()
třídní metoda vrací složené bitové příznaky nepovinných vlastností, které Vaše implementace zvládá a které mají být testované. Vracená hodnota vznikne jako kombinace konstant SPREADSHEET_xxx, případně 0, pokud nechcete provádět žádný nepovinný test. Pokud Vaše implementace zadanou vlastnost zvládá, ale v návratové hodnotě capabilities() ji neuvedete, pak se vlastnost nebude testovat a nedostanete za ni body. Pokud Vaše implementace nějakou vlastnost nezvládá, ale ponecháte ji zapnutou v capabilities, pak se vlastnost testovat bude a test selže. Navíc, pokud při testu Váš program spadne nebo překročí časový limit, nebudou prováděné další testy, tedy můžete být ohodnoceni hůře. Je proto rozumné, aby vracená hodnota korespondovala se skutečně implementovanými vlastnostmi. Konstanty mají následující význam:
SPREADSHEET_CYCLIC_DEPS - implementace si poradí s cyklickými závislostmi mezi buňkami,
SPREADSHEET_FUNCTIONS - implementace zpracovává funkce ve výrazech,
SPREADSHEET_FILE_IO - implementace dokáže ukladat a načítat do/ze souborů a poradí si s poškozenými soubory,
SPREADSHEET_SPEED - výpočty hodnot buněk jsou optimalizované, má smysl provádět nepovinný test rychlosti,
SPREADSHEET_PARSER - implementace používá vlastní parser výrazů, má smysl provádět bonusový test.
Co znamenají jednotlivé testy:
Základní test podle ukázky - provede sadu testů, které jsou v dodaném souboru.
Test výrazů bez cyklických závislostí - vyplní tabulku hodnotami a výrazy, zkontroluje správnost vypočtených výsledků. Nezadávají se výrazy, které obsahují cyklické závislosti a dále ve výrazech nejsou použité funkce (sum, if, count, ...).
Test save/load/copyRect - kopíruje se blok buněk, v kopírovaném bloku jsou absolutní i relativní odkazy na jiné buňky. Kontroluje se, že po zkopírování jsou odkazy správně přepočtené. Dále se kontroluje, zda se tabulka se zkopírovanými buňkami správně ukládá a načítá.
Test rychlosti vyhodnoceni (AST) - kontroluje se, že vyhodnocení buněk používá nějakou rozumnou implementaci (např. AST). Test mj. selže, pokud se pokusíte buňky vyhodnocovat opakovaným voláním parseExpression().
Test cyklických závislostí - vyplněné výrazy obsahují cyklické závislosti. Program je musí umět detekovat, nesmí skončit v nekonečné smyčce/rekurzi.
Test implementace funkcí - kontroluje se, že program implementuje funkce a správně pracuje s rozsahy buněk.
Test load/save - vytvořená tabulka je uložena a následně nahrána zpět. V tomto procesu je obsah uloženého souboru někdy "poškozen". Kontroluje se, že Vaše implementace dokáže rozpoznat poškozený soubor a oznámí chybu při pokusu o jeho načtení.
Test rychlosti - vytvořená tabulka je často přepočítávána. Kontroluje se, zda je výpočet rozumně rychlý (často se počítá obsah buněk a často se mění hodnoty v buňkách, významně méně často se mění vzorce v buňkách).
Co a jak implementovat:
Odevzdané řešení nemusí splnit celou popsanou funkcionalitu. Je rozumné řešení postupně rozšiřovat. Před vlastní implementací se seznamte s tím, jak se používají staticky linkované knihovny a jak funguje AST. Teprve pak začněte s návrhem svých tříd. Dobře si rozmyslete, jak se na návrhu tříd projeví požadavek na kopírování buněk a jak zařídíte přepočet absolutních/relativních odkazů. Návrh tříd nepodceňujte!
Povinné testy vyžadují vyplňování buněk, vkládání vzorců (bez funkcí a rozsahů buněk) a kopírování. Lze použít dodaný parser výrazů.
Samotné zvládnutí povinných testů nestačí pro překonání bodového minima, budete muset implementovat nějakou další funkcionalitu a projít nějakým nepovinným testem. Doporučujeme kontrolu cyklických závislostí, za tento test získáte nejvíce bodů.
Úloha nabízí bonusový test (poslední test), kdy se předpokládá implementace vlastního parseru. Bonus sice je nabízen, ale studenty od něj spíše odrazujeme. Referenční řešení úlohy, které projde všemi testy bez bonusu, má cca 1500 řádek kódu (cca 75 KiB). Implementace parseru přidá dalších cca 700 řádek kódu (cca 25 KiB), to za maximálně 2 body navíc. POZOR: pokud se rozhodnete pro implementaci vlastního parseru, nepoužívejte pro jeho rozhraní identifikátor parseExpression(). Funkce parseExpression() v testovacím prostředí existuje, i pokud se rozhodnete pro vlastní parser. Pokud byste svůj parser pojmenovali stejně, bude se hlásit chyba při kompilaci (linkování).
Co odevzdávat:
Očekává se odevzdání jednoho souboru, ve kterém jsou umístěné všechny potřebné třídy s řešením (CSpreadsheet, CPos, ... a Vaše pomocné třídy a funkce). Takový soubor je velmi dlouhý a nepřehledný.
Proto pracujeme na úpravě, která by dokázala pracovat s více zdrojovými soubory. Vstupem by pak byl archiv se zabalenými soubory nebo integrace s fakultním gitlabem. V tomto okamžiku takové řešení není funkční, budeme se jej snažit včas zprovoznit.
Doporučujeme proto řešení vyvíjet ve více oddělených souborech (.cpp, .h) a používat fakultní gitlab, kam si budete ukládat rozpracovaný program. I pokud by se nám úpravu nepodařilo včas dokončit, vždy lze oddělené soubory spojit do jednoho a odevzdat (grep -vh '^#include' file1.h file2.h ... fileN.cpp > all_in_one.cpp). Takový postup samozřejmě není ideální, proto se budeme snažit včas zprovoznit lepší řešení.