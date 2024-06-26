CRange =(N)celociselny interval
-jedno cislo -pr.42
- vice cisel -pr.<1..2>
-ukladat jako longlongint
-kontruktor (lo,hi)
    -if lo>hi throw std::logic_error



CRangeList = seznam intervalu
-sousedící nebo překrývající se 
    intervaly sloučíme do jednoho delšího intervalu

-kontruktor ini.prazdny seznam intervalu
(-kopirujici konstruktor
-destruktor
-op=)
include ll a include CRange
    -metody zjistí, 
    zda seznam intervalů obsahuje 
    zadanou hodnotu / celý zadaný 
    interval hodnot. Pokud ano, 
    vrací true, pokud ne, vrací false.
-op+ a op- CRange
-op+=
    -pridani intervalu
    -slucuje
/Sloučí se ale i intervaly
 <10..20> a <21..30>, přestože
  se nedotýkají ani nepřekrývají.
   Dohromady však pokrývají celý 
   interval <10..30>. 
   Intervaly <10..19> a <21..30> 
   se už ale nesloučí, nepokrývají
    číslo 20./

-op-=
/pomocí tohoto operátoru se budou odebírat další intervaly ze seznamu. Odebírání lze chápat i jako množinový rozdíl. Operátor zajistí, že po odebrání budou intervaly uloženy kompaktně (odstraňování nepotřebných intervalů)./

-op=
/
pomocí tohoto operátoru lze nastavit obsah na seznam intervalů na pravé straně (samozřejmě kompaktně uložený)./

-op== a op!=
/operátory porovnají obsah dvou instancí. Za shodné považujeme dvě instance, které mají identický seznam intervalů./

-op<<
/operátor zobrazí obsah seznamu do zadaného výstupního streamu. Seznam bude uzavřen ve složených závorkách, jednotlivé intervaly oddělené čárkou. Jednoprvkové intervaly budou zobrazené jako číslo, delší intervaly pak v lomených závorkách. Formát výpisu je zřejmý z ukázek. Intervaly budou zobrazeny seřazené vzestupně./