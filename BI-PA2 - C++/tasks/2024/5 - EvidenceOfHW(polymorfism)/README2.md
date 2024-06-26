Úloha je rozšířením jednodušší úlohy Evidence výpočetní techniky o možnosti virtualizace a cloudových řešení. Úkolem je navrhnout a implementovat sadu tříd, které budou simulovat evidenci počítačového vybavení firmy. Konkrétně budeme ukládat informace o sítích (CNetwork), počítačích (CComputer), jejich procesorech (CCPU), pamětech (CMemory) a discích (CDisk). Oproti jednodušší úloze půjde modelovat virtualizované sítě a počítače, tedy počítače a sítě, které jsou obsažené v jiném počítači. Vytvořené počítače a sítě navíc půjde kopírovat.

Úkol je zaměřen na návrh tříd, kde bude využito dědičnosti, polymorfismu a abstraktních metod. Pokud jsou tyto OOP prostředky použité racionálně, není implementace příliš dlouhá. Naopak, pokud provedete návrh špatně, bude se Vám kód opakovat a implementační soubor bude velký. Zkuste identifikovat základní třídu a vhodně z ní děděním odvoďte podtřídy.

Třídy a jejich rozhraní:

CNetwork
reprezentuje síť. Její rozhraní musí obsahovat:
konstruktor se jménem sítě,
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metodu addComputer, kterou lze přidávat další počítač do sítě,
metodu findComputer, která vrátí odkaz na nalezený počítač zadaného jména nebo neplatý odkaz, pokud jej nenalezne. Pozor, hledá se i ve virtuálních počítačích, které např. běží uvnitř jiného počítače, který je připojen k této síti,
metodu findNetwork, která vrátí odkaz na nalezenou síť zadaného jménanebo neplatý odkaz, pokud ji nenalezne. Pozor, hledá se i ve virtuálních sítích, které např. běží uvnitř jiného počítače, který je připojen k této síti,
výstupní operátor, který zobrazí strom počítačů a komponent, jako v ukázce. Počítače jsou vypsané v pořadí přidávání.
CComputer
reprezentuje počítač. Její rozhraní musí obsahovat:
konstruktor s parametrem jména počítače
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metoda addComponent, která přidá další komponentu počítače,
metoda addAddress, která přidá další adresu počítače (řetězec),
metodu findComputer, která vrátí odkaz na nalezený počítač zadaného jména nebo neplatý odkaz, pokud jej nenalezne. Výsledkem může být tento počítač sám nebo i virtuální počítač, který běží na tomto počítači,
metodu findNetwork, která vrátí odkaz na nalezenou síť zadaného jménanebo neplatý odkaz, pokud ji nenalezne. Pozor, hledá se i ve virtuálních sítích, které např. běží uvnitř tohoto počítače,
metoda duplicate(remap), která vytvoří kopii tohoto počítače. Vytváří se identická kopie (až na případné změny jména), včetně případných virtualizovaných počítačů/sítí. Při vytváření kopie metoda navíc změní jména všech kopírovaných počítačů a sítí podle parametru (remap). Tento parametr je tvořen dvojicemi řetězců (původní jméno, nové jméno). Pokud se jméno kopírovaného počítače/sítě objeví v parametru remap jako nějaké původní jméno, bude nahrazeno. Pokud původní jméno v parametru obsaženo není, bude ponecháno beze změny.
operátor pro výstup, který zobrazí přidělená adresy a komponenty počítače, jako v ukázce. Ve výpisu jsou nejprve uvedené adresy (v pořadí zadání) a za nimi komponenty (v pořadí přidávání).
CCPU
reprezentuje CPU. Její rozhraní musí obsahovat:
konstruktor s parametrem počtu jader (celé číslo) a frekvencí (celé číslo v MHz),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace).
CMemory
reprezentuje RAM. Její rozhraní musí obsahovat:
konstruktor s parametrem velikosti paměti (celé číslo v MiB),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace).
CDisk
reprezentuje úložiště. Její rozhraní musí obsahovat:
konstruktor s parametry typu disku (symbolická konstanta SSD nebo MAGNETIC deklarovaná ve třídě) a velikosti disku (celé číslo v GiB),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metodu addPartition, která přidá informaci o rozdělení disku. Metoda bude mít dva parametry - velikost parcely v GiB a její identifikaci (řetězec). Výpis parcel je v pořadí zadávání.
Odevzdávejte zdrojový kód se implementací tříd CNetwork, CComputer, CCPU, CMemory a CDisk. Do odevzdávaného souboru zahrňte všechny potřebné podpůrné deklarace. Části vkládání hlaviček a Vaše testy ponechte v bloku podmíněného překladu, jak je ukázáno v přiložené ukázce.

Poznámky
Používejte operátory pro přetypování (dynamic_cast) s rozmyslem. Referenční implementace v sobě nemá žádné přetypování ani žádné použití RTTI. Obecně, RTTI, dynamic_cast a typeid vedou k více větvenému kódu, který je hůře čitelný a hůře rozšiřitelný. Navrhněte třídy tak, abyste si vystačili s polymorfismem.
Všimněte si, že v ukázce chybí hlavičkový soubor typeinfo, tedy operátor typeid nelze používat.
Vaše řešení musí používat třídy, třídy musí tvořit hierarchii, dědičnost a polymorfismus musí být použité. V této úloze je použití dědění a polymorfismu vhodné, navíc, testovací prostředí odmítne řešení, které by dědění, polymorfismus a dynamicky vázané metody nevyužívalo (takové řešení bude odmítnuto na chybě při kompilaci).