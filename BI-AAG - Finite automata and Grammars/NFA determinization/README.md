Algoritmus determinizace konečného automatu je klíčovým nástrojem v teorii formálních jazyků.
 Nalézá rozsáhlé praktické využití ve zpracování textu, analýze dat a v různých oblastech
  informatiky. Tím, že konvertuje nedeterministické automaty na deterministické, umožňuje
 efektivní analýzu jazyků, což je klíčové v kompilátorech, analýze textu, vyhledávání
 regulárních výrazů, řízení sítí, databázových systémech a mnoha dalších aplikacích.

UKOL:

Úkolem je najít DKA bez zbytečných a nedosažitelných stavů
 takový, aby jazyk, který přijímá byl ekvivalentní k jazyku NKA s více počátečními stavy.

Determinizaci implementujte v podobě funkce programu v C++, jejíž signatura je: DFA determinize
 ( const MISNFA & nfa );.
Vstupem: automaty v podobě struktur
         MISNFA
Vystupem: DFA reprezentující NKA s více
           poč. stavy, resp. DKA

Tyto struktury jsou definovány v
 testovacím prostředí, viz ukázka níže. Pro zjednodušení jsou stavy definovány jako hodnoty
 typu int a symboly abecedy jako hodnoty typu char.

Porovnání automatů s referenčním výsledkem se provádí přes převod na minimální deterministický
 konečný automat. Vaše výstupy se mohou lišit (např. v pojmenování stavů). Po převedení na
 minimální automat, které provede testovací prostředí (tj. minimalizace není vaší starostí),
 však musí dát ekvivalentní automat (pojmenování stavů nehraje roli).

Je zaručeno, že na vstupu přijde validní NKA s více počátečními

 stavy, tedy:

množiny stavů (MISNFA::m_States), počátečních stavů (MISNFA::m_InitialStates) a symbolů abecedy
 (MISNFA::m_Alphabet) budou neprázdné,
počáteční a koncové stavy z množin MISNFA::m_InitialStates a MISNFA::m_FinalStates budou také
 prvky množiny stavů MISNFA::m_States,
pokud nebude pro nějaký stav q a symbol abecedy a definovaný přechod v automatu, pak v mapě
MISNFA::m_Transitions nebude ke klíči (q, a) přiřazená hodnota prázdná množina, ale tento klíč
 nebude vůbec existovat,
v mapě přechodů MISNFA::m_Transitions se vyskytují také jen prvky, které jsou specifikovány v
 množině symbolů abecedy a stavů.
Výsledný DFA musí také splňovat podmínky definice automatu, tedy musí platit to samé co výše
 pro MISNFA (až na zřejmé změny kvůli rozdílným definicím počátečního stavu a
 přechodové funkce).

V případě, že jazyk automatu je prázdný, odevzdávejte jednostavový automat nad stejnou
 abecedou, jako je původní automat.

Pro základ implementace můžete využít soubor ke stažení níže v sekci Vzorová data. Tento
 soubor obsahuje také několik základních testů, mějte však na paměti, že výsledky Vaší
 ímplementace se mohou lišit. Testy jsou nastaveny podle výsledků, které dává jedno z
 referenčních řešení. Možná si je tedy budete muset upravit.
