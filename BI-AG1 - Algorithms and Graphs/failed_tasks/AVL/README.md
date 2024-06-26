Vaším úkolem je implementovat třídu TextEditorBackend, která
představuje backend textového editoru. Požadované rozhraní:

TextEditorBackend(const std::string& text): konstruktor, který
 inicializuje obsah editoru daným textem.

size_t size() const: #znaků textu včetně nových řádků.

size_t lines() const:  #řádků. Prázdný text má jeden řádek.
Každý znak \n přidá řádek.

char at(size_t i) const: Vrátí i-tý znak.
Povolený rozsah je [0, size()).

void edit(size_t i, char c): Nahradí znak na pozici i znakem c.
Povolený rozsah je [0, size()).

void insert(size_t i, char c): Vloží znak c před znak na pozici i.
Povolený rozsah je [0, size()].

void erase(size_t i): Smaže znak na pozici i.
Povolený rozsah je [0, size()).

size_t line_start(size_t r) const:
Vrátí začátek r-tého řádku.
Znak \n je součástí řádku, který ukončuje. Povolený rozsah je [0, lines()).

size_t line_length(size_t r) const: Vrátí délku i-tého řádku
včetně případného znaku \n na konci. Povolený rozsah je [0, lines()).

size_t char_to_line(size_t i) const:
Vrátí číslo řádku, na kterém se nachází i-tý znak.
Povolený rozsah je [0, size()).

Pokud je metoda volána s argumentem mimo rozsah, musí vyvolat
výjimku std::out_of_range.
Metody size a lines by měly O(n).
Ostatní metody by měly mít O(logn).
Konstruktor by neměl být pomalejší, než n*insert.