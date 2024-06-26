# Aplikace: Poznámkovník

## Progtestove zadani

Vaším úkolem je vytvořit program pro správu poznámek, seznamů a připomínek.

### Implementujte následující funkcionality:

Vytvořte alespoň 3 druhy poznámek (složka poznámek se nepočítá, př. textová, nákupní seznam, todo list, tabulka,
myšlenková mapa, …).

Poznámky jsou umisťovány do adresářů, které mohou mít podadresáře. Pro každou poznámku je možné určit název, datum
vytvoření, štítky (tagy). Pro jednotlivé druhy pak další potřebné atributy.

Umožněte vytvářet, zobrazovat, upravovat a mazat formátované poznámky. Veškeré úpravy a změny evidujte v logu aktivit u
jednotlivých poznámek.

Umožněte export poznámek do nějakého standardního formátu (př. HTML, Markdown, AsciiDoc, …).

Umožněte vyhledávat poznámky podle názvu, data vytvoření, štítků, adresáře nebo textu v nich obsaženého a jejich
kombinací (př. v adresáři "škola" se štítky "PA1" nebo "PA2", …).

Umožněte poznámky exportovat a následně importovat (vytvořte vhodný textový formát). Export musí podporovat všechny či
podmnožinu určenou vyhledáváním.

### Kde lze využít polymorfismus? (doporučené)

druhy poznámek: složka poznámek, textová poznámka, seznam úkolů, nákupní seznam (zobrazuje celkovou cenu), tabulka,
myšlenková mapa, …

filtry k vyhledávání: (ne)obsahuje text, a zároveň, nebo, procento úkolů splněno

různé formáty importu a exportu: proprietární, HTML, Markdown, AsciiDoc, …

různá kritéria řazení poznámek: podle data vytvoření, podle abecedy, …

uživatelské rozhraní: konzole, ncurses, SDL, OpenGL (různé varianty), …

Další informace:

https://guides.github.com/features/mastering-markdown/

Pokud chcete svůj poznámkovník rozšířit, můžete umožnit uživateli řadit poznámky nebo je šifrovat.

## Specifikace zadani

### Implementace funcionalit

Tato aplikace bude umožňovat vytváření, zobrazování a úpravu
3 různých druhů poznámek - textová, todolist (kde úkoly budou splněné/nesplněné)
a shopping list (kde potraviny budou mit počet kusů a cena za kus).
Každá poznámka má název, datum vytvoření, libovolné množství tagů a log aktivit.

Log aktivit zaznamenává operace nad danou poznámkou.
Konkrétní operace jsou:
vytvoření a mazání úkolu nebo položky pro nákup,
úprava nazvu/úkolu/položky/textu,
zobrazení,
přidání, odebrání tagu

Poznámky se budou moct seskupovat do složek, pričemz se složky do sebe mohou zanořovat.

Poznámky bude možné vyhledávat podle názvu, data vytvoření,
tagu, adresáře nebo textu v nich obsaženého a jejich
kombinací (př. v adresáři "škola" se štítky "PA1" nebo "PA2", …).

Poznamkovnik se bude vykreslovat pomoci knihovny ncurses.

Aplikace bude umožňovat import z Markdownu a export do Markdownu, kde to bude navíc
možno exportovat podle vyhledavání v poznámkach.

### Využití polymorfismu

#### Na druzích poznámek:

Třídy todolist, text i shopping-list budou dědit z nadřazené třídy note.
Třída folder s třídou note budou dědit nadřazené třídy file_manager.
Tím bude možné si v každé intanci folder udržovat v
jednom kontejneru jak foldery, tak i noty.

Potomci file_manager budou implementovat polymorfní
metody pro zpracování vstupu, mazání, hledání, upravení formátu pro export.

#### Na menu, které zobrazuje možnosti pro vybranou složku/poznámku:

Potomci left_menu budou implementovat polymorfní metody
pro vypsání možností a posunutí indexu vybrané možnosti.

### Ovládání

ESC - vrácení akce

ENTER - potvrzení akce

E - zobrazení menu s možnostmi

Při importu napište pouze jméno souboru a při exportu přidejte i koncovku ".md".
Export najdete ve slozce example/.

Makefile postup:

1. make all
2. make run

### Nedostatky

- persistence dat
- hledani
- makefile neraguje na zmenu .h
- nazvy jako CFileManager
- const stringy by mely byt constexpr static const
- opakovani fci kvuli cyklickym ref a sharedfromthis