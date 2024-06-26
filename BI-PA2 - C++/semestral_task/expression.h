#ifndef expression_h_09845924528375
#define expression_h_09845924528375

#include <string>
//CExpressionBuilder - abstraktní třída, kterou používá dodaný analyzátor výrazů. Pokud se
//rozhodnete používat dodaný analyzátor, budete muset implementovat podtřídu a její instanci předat analyzátoru.
//parseExpression() - funkce v testovacím prostředí, která provede syntaktickou analýzu
//zadaného výrazu a identifikované části tohoto výrazu předá ke zpracování Vaší podtřídě CExpressionBuilder.

/**
Funkce parseExpression( expr, builder ) představuje rozhraní dodaného analyzátoru výrazů,
 které lze použít v buňkách. Parametrem volání je řetězec s výrazem expr a instance builder,
 jejíž rozhraní bude voláno během analýzy výrazu. Funkce nevrací žádnou hodnotu,
 zpracovaný výraz si bude v sobě ukládat dodaná instance builder. Funkce výraz buď zpracuje bez chyb,
 nebo vyhodí výjimku std::invalid_argument s popisem chyby.
 Pro správné použití funkce je nutné dodat vlastní builder.
 Ten vznikne jako potomek abstraktní třídy CExpressionBuilder, realizace této podtřídy je Váš úkol.

Syntaktická analýza výrazu vytvoří z infixového zápisu (např. 4 + A2 + B$7) jeho postfixovou podobu,
 tedy pro ukázkový příklad to bude: 4 A2 + B$7 +. Tato postfixová podoba se nevytváří explicitně.
 Parser místo toho volá odpovídající metody builderu, pro ukázku by volal:

valNumber (4)
valReference ("A2")
opAdd ()
valReference ("B$7")
opAdd ()
Postfixová podoba je mnohem jednodušší na zpracování. Dodávané hodnoty operandů
 (valNumber, valString, valReference, ...) si ukládejte na zásobník ve své instanci podtřídy
 CExpressionBuilderu. Přicházející operátor (opAdd, opSub, ..., funcCall) znamená,
 že si vyzvednete odpovídající počet operandů ze svého zásobníku a tyto operandy
 spojíte odpovídajícím operátorem (např. operátorem +). Musíte dávat pozor na pořadí operandů,
 levější operandy jsou na zásobníku hlouběji. Výsledek si uložíte na zásobník.
 Po úspěšném zpracování celého výrazu budete mít na zásobníku jedinou položku - vyhodnocení celého výrazu.

Rozhraní dodaného parseru je flexibilní. Umožní například vyhodnocovat výraz rovnou během syntaktické analýzy
 - na zásobník budete ukládat hodnoty vyhodnocených podvýrazů.
 Tedy opakovaným zpracování výrazu v buňce lze vypočítat novou hodnotu výrazu
 (např. po změně obsahu nějaké buňky, na které hodnota výrazu záleží).
 Takový postup je ale velmi pomalý. Proto využijte dodaný parser efektivněji
 - použijte jej pro konstrukci AST. Opakované vyhodnocení AST je režijně mnohem méně náročné
 než opakované parsování celého výrazu.
 */
class CExprBuilder
{
public:
    //aplikace operátoru sčítání/zřetězení (+),
    virtual void opAdd() = 0;
    //aplikace operátoru odčítání (-)
    virtual void opSub() = 0;
    //aplikace operátoru násobení (*)
    virtual void opMul() = 0;
    //aplikace operátoru dělení (/)
    virtual void opDiv() = 0;
    //aplikace operátoru umocnění (^),
    virtual void opPow() = 0;
    //aplikace operátoru unární mínus (-),
    virtual void opNeg() = 0;
    //aplikace operátoru porovnání na rovnost (=),
    virtual void opEq() = 0;
    //aplikace operátoru porovnání na nerovnost (<>)
    virtual void opNe() = 0;
    //aplikace operátoru porovnání menší než (<),
    virtual void opLt() = 0;
    //aplikace operátoru porovnání menší nebo rovno (<=),
    virtual void opLe() = 0;
    //aplikace operátoru porovnání větší než (>),
    virtual void opGt() = 0;
    //aplikace operátoru porovnání větší nebo rovno (>=),
    virtual void opGe() = 0;
    //přidání hodnoty číselného literálu, parametrem je hodnota čísla,
    virtual void valNumber(double val) = 0;
    //přidání hodnoty řetězcového literálu, parametrem je hodnota řetězce,
    virtual void valString(std::string val) = 0;
    //přidání odkazu na jinou buňku, parametrem je identifikátor buňky (např. A7 nebo $x$12),
    virtual void valReference(std::string val) = 0;
    //přidání odkazu na rozmezí buněk (range), parametrem je identifikace obdélníkového výřezu buněk
    // (např. A7:$C9 nebo $x$12:z29),
    virtual void valRange(std::string val) = 0;

    /**
     * přidání volání funkce, parametrem je jméno funkce fnName a počet parametrů funkce paramCnt.
     * Dodaný parser kontroluje, zda je zadané jméno funkce platné
     * (sum, min, max, count, if nebo countval) a dále kontroluje,
     * že počty a typy parametrů souhlasí (např. že if má 3 parametry a žádný není range,
     * sum musí mít jeden parametr typu range, ...).
     * Tedy tuto kontrolu nemusíte na straně builderu provádět.
     * Kontrola je samozřejmě potřeba, pokud se rozhodnete pro vlastní parser.
     */
    virtual void funcCall(std::string fnName, int paramCount) = 0;
};

void parseExpression(std::string expr, CExprBuilder &builder);

#endif /* expression_h_09845924528375 */
