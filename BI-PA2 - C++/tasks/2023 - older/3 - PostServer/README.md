CMail
-adresa sender,adresa taker,zprava

CMailServer
sendMail ( mail )
zašle e-mail předaný v parametrech, 
efektivně jej zařadí do odpovídajících schránek odesílatele
 a příjemce. E-mail je vždy zařazen na konec existujícího seznamu. 
 Příjemce ani odesílatele není potřeba zakládat, schránka se 
 automaticky vytvoří po zpracování prvního e-mailu, 
 který obsahuje danou e-mailovou adresu,

outbox ( email )
zpřístupní poštu odeslanou ze zadané adresy.
Návratovou hodnotou je instance CMailIterator,
která umožní procházet emaily odeslané z adresy email.
Pokud je zadaná neznámá e-mailová adresa, je výsledkem 
iterátor pro prázdný seznam e-mailů. Vrácený iterátor
musí zachycovat stav mailové schránky v okamžiku, kdy 
byl vytvořen. Tedy pokud během používání iterátoru dojde
 ke změně obsahu procházené schránky, tato změna se do hodnot
 vracených iterátorem nijak nepromítne. Toto chování je 
demonstrované v ukázkovém běhu např. pro iterátor i5.

inbox ( email )
zpřístupní poštu přijatou na zadanou adresu. 
Jinak metoda pracuje stejně jako metoda outbox.
emailové adresy
v úloze mohou být libovolné řetězce, při jejich 
porovnávání rozlišujeme malá a velká písmena (case sensitive)
 - v tomto se úloha liší od reálného světa, kde e-mailové 
 adresy mají předepsaný formální tvar a kde se malá a velká 
 písmena zpravidla nerozlišují.


Požadovaná třída CMailIterator má následující rozhraní:

operator bool
operátor zjistí, zda iterátor odkazuje na platný e-mail (vrací true),
 nebo zda dosáhl za poslední e-mail v seznamu (tedy e-mail už nelze
  číst, vrátí false),

operator !
funguje stejně jako předešlý operátor, pouze vrací opačnou 
návratovou hodnotu

operator *
unární operátor * zpřístupní e-mail na aktuální pozici.
 Návratovou hodnotou je instance CMail (případně konstantní
  reference na CMail). Nemusíte řešit situaci, že by se zpřístupnil 
e-mail za koncem seznamu - testovací prostředí vždy nejprve kontroluje platnost iterátoru a teprve pak případně zpřístupní odkazovaný e-mail.

operator ++
prefixový operátor ++ zajistí přesun iterátoru na další e-mail v seznamu. E-maily jsou iterátorem procházené v pořadí, ve kterém byly odeslané/přijaté. Opakovaným voláním tohoto iterátoru se lze přesunout od prvního e-mailu přijatého/odeslaného zadanou e-mailovou adresou až k poslednímu (pak musí operátor přetypování na bool vracet false).

kopírující konstruktor, operator =, destruktor
podle způsobu implementace možná nebude postačovat 
automaticky generovaná varianta. Testovací prostředí 
iterátory nikde explicitně nekopíruje, ale ke kopírování 
dochází v okamžiku předávání návratové hodnoty metodami inbox a outbox.

