# Úkol 3: Klasifikace (Originál vs. Inpainted), Interpretovatelnost (XAI) a Siamské sítě

## 1. Příprava datasetu a inpainting

Cílem první části bylo vytvořit z dostupných mikroskopických snímků řasinek vyvážený dataset pro binární klasifikaci do dvou tříd:
- **Třída 0 (Original):** Původní snímky řasinek s přirozenou strukturou periferních mikrotubulových dubletů a centrálního páru.
- **Třída 1 (Inpainted):** Modifikované snímky vzniklé odstraněním (vyretušováním) mikrotubulů pomocí inpaintingu.

### 1.1 Metodika inpaintingu (OpenCV Telea)
Pro inpainting mikrotubulů byly využity bodové masky z `Data/masks_pts/`. Bodové anotace byly mírně dilatovány eliptickým strukturním elementem ($3 \times 3$ px), aby plně pokryly oblast mikrotubulů i s jejich bezprostředním okolím. Následně byl aplikován inpaintingový algoritmus Alexandru Telea (`cv2.inpaint` s poloměrem $r = 3$ px, `cv2.INPAINT_TELEA`). 

Tento algoritmus rekonstruuje chybějící oblast pomocí řešení parciálních diferenciálních rovnic šířením gradientu a izofot z okolí, čímž vzniká realisticky vyhlazená textura bez ostrých přechodů.

![](/IMGS3/dataset_samples.png)

*Obrázek 1: Ukázka zpracování datasetu – zleva doprava: originální řasinka, bodová maska mikrotubulů, inpaintovaný snímek (odstraněné tubuly) a absolutní diferenční mapa ilustrující modifikovanou oblast.*

### 1.2 Rozdělení datasetu (Train / Validation Split)
Dataset obsahuje celkem **352 unikátních buněk**. Pro trénování a evaluaci bylo klíčové provést rozdělení na **úrovni buněk** (cell ID) v poměru **80 : 20**:
- **Trénovací sada:** 281 buněk $\rightarrow$ **562 snímků** (281 originálních + 281 inpaintovaných)
- **Validační sada:** 71 buněk $\rightarrow$ **142 snímků** (71 originálních + 71 inpaintovaných)

> [!IMPORTANT]
> Rozdělení podle ID buněk zajišťuje, že se originál i jeho inpaintovaná varianta vždy nacházejí ve stejném splitu. Tím je striktně eliminován únik informací (data leakage) mezi tréninkovou a validační sadou a modely jsou vyhodnocovány výhradně na neviděných buňkách.

### 1.3 Předzpracování a augmentace
- **Rozměry a formát:** Snímky jsou přeškálovány na jednotné rozlišení $128 \times 128$ px a převedeny na 3-kanálový formát RGB pro kompatibilitu s páteřní sítí ResNet18.
- **Normalizace:** Hodnoty pixelů jsou převedeny do plovoucí řádové čárky $[0.0, 1.0]$ a standardizovány pomocí průměrů a směrodatných odchylek ImageNetu ($\mu = [0.485, 0.456, 0.406]$, $\sigma = [0.229, 0.224, 0.225]$).
- **Trénovací augmentace:** Náhodné horizontální překlopení ($p = 0.5$) a náhodné vertikální překlopení ($p = 0.5$).

---

## 2. Klasifikace: Vlastní CNN vs. Předtrénovaný ResNet18

Byla provedena komparativní analýza dvou odlišných přístupů:
1. Návrh vlastní konvoluční sítě od nuly (Custom CNN).
2. Přenos učení (Transfer Learning) s využitím modelu ResNet18 předtrénovaného na datasetu ImageNet.

### 2.1 Architektura vlastní CNN
Vlastní architektura byla navržena s důrazem na hierarchickou extrakci příznaků:
- **4 konvoluční bloky:**
  - Blok 1: $\text{Conv2D}(3 \rightarrow 32, 3 \times 3, \text{pad}=1) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU} \rightarrow \text{MaxPool2D}(2)$ ($128 \rightarrow 64$)
  - Blok 2: $\text{Conv2D}(32 \rightarrow 64, 3 \times 3, \text{pad}=1) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU} \rightarrow \text{MaxPool2D}(2)$ ($64 \rightarrow 32$)
  - Blok 3: $\text{Conv2D}(64 \rightarrow 128, 3 \times 3, \text{pad}=1) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU} \rightarrow \text{MaxPool2D}(2)$ ($32 \rightarrow 16$)
  - Blok 4: $\text{Conv2D}(128 \rightarrow 256, 3 \times 3, \text{pad}=1) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU} \rightarrow \text{AdaptiveAvgPool2D}(1 \times 1)$
- **Klasifikační hlava:**
  - $\text{Flatten} \rightarrow \text{Dropout}(0.3) \rightarrow \text{Linear}(256 \rightarrow 128) \rightarrow \text{ReLU} \rightarrow \text{Dropout}(0.3) \rightarrow \text{Linear}(128 \rightarrow 2)$
- **Trénovací parametry:** Optimalizátor Adam, learning rate $\eta = 10^{-3}$, weight decay $10^{-4}$, velikost dávky 32, 10 epoch.

### 2.2 Architektura ResNet18 (Transfer Learning)
Předtrénovaný ResNet18 disponuje reziduálními bloky se skip connections. Poslední plně propojená vrstva (původně 1000 tříd) byla nahrazena novou klasifikační hlavou:
$$\text{fc} = \text{Sequential}(\text{Dropout}(0.2), \text{Linear}(512 \rightarrow 2))$$
Model byl jemně doladěn (fine-tuning) s nižším learning rate $\eta = 10^{-4}$ pro zachování stability vah naučených na ImageNetu.

### 2.3 Srovnání průběhu trénování

![](/IMGS3/models_comparisonFreeze.png)
![](/IMGS3/models_comparisonFine.png)

### 2.4 Kvantitativní vyhodnocení a matice záměn

Na validační sadě (142 snímků – 71 originálních a 71 inpaintovaných) bylo dosaženo následujících výsledků:

| Model | Val Loss | Accuracy | Precision | Recall | F1-Score |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Vlastní CNN (Custom)** | ~0.20 - 0.31 | **88.73 %** | **88.00 %** | **89.74 %** | **0.8870** |
| **Předtrénovaný ResNet18** | **0.0258** | **99.30 %** | **100.00 %** | **98.59 %** | **0.9929** |

![](/IMGS3/confusion_matrices.png)

*Obrázek 3: Matice záměn obou modelů na nezávislé validační sadě.*

#### Diskuse výsledků klasifikace:
- **ResNet18** vykazuje mimořádnou rozlišovací schopnost. Během pouhých 4 epoch konverguje k validační přesnosti **99.30 %** (pouze jediný chybně klasifikovaný vzorek ze 142). Schopnost detekovat jemné nepravidelnosti po inpaintingu pramení z bohatých nízkých a středních příznaků naučených na rozsáhlém datasetu ImageNet.
- **Vlastní CNN** dosahuje stabilní přesnosti **~89 %**. Vzhledem k trénování od nuly na relativně malé datové sadě (562 vzorků) se jedná o velmi dobrý výsledek. Využití BatchNorm a Dropoutu (0.3) úspěšně zamezilo závažnému přetrénování.

---

## 3. Interpretovatelnost modelu (XAI - Explainable AI)

Pro hloubkovou analýzu rozhodovacího mechanismu klasifikačních modelů byly implementovány a porovnány tři komplementární atribuční metody:
1. **Grad-CAM (Gradient-weighted Class Activation Mapping):** Hrubá lokalizační metoda, která váží příznakové mapy poslední konvoluční vrstvy (`layer4` u ResNetu a 4. konvoluční blok u vlastní CNN) pomocí globálně průměrovaných gradientů cílové třídy. Následná aplikace ReLU propouští pouze příznaky s pozitivním vlivem na skóre.
2. **Vanilla Saliency Maps:** Měří lokální citlivost modelu jako absolutní hodnotu gradientu predikované třídy vzhledem ke vstupním pixelům: $S(x) = \max_c |\partial y_{\text{target}} / \partial x_c|$.
3. **Integrated Gradients:** Axiomaticky podložená metoda počítající integrál gradientů podél přímé linie spojující nulový referenční obraz (baseline $x_0$) a vstupní vzorek $x$:
   $$\text{IG}_i(x) = (x_i - x_{0,i}) \times \int_{0}^1 \frac{\partial F(x_0 + \alpha(x - x_0))}{\partial x_i} d\alpha$$
   V implementaci byl integrál aproximován Riemannovým součtem o 25 krocích.

### 3.1 Výsledky interpretace pro ResNet18

![](/IMGS3/xai_samples_resnetFreeze.png)
![](/IMGS3/xai_samples_resnetFine.png)

### 3.2 Výsledky interpretace pro vlastní CNN

![](/IMGS3/xai_samples_custom.png)

*Obrázek 5: XAI analýza pro vlastní konvoluční síť.*

### 3.3 Diskuse zaměření modelů (Focus Areas):
- **Originální řasinky (Original):**
  - Modely se primárně zaměřují na **vnitřní mikrotubuly** (centrální pár a uspořádání periferních dubletů).
  - V mapách Grad-CAM vidíme koncentraci vysoké aktivity uprostřed axonemy.
  - Saliency a Integrated Gradients přesně kopírují kruhové hrany mikrotubulů, což potvrzuje, že síť správně vnímá přítomnost tubulárních struktur jako klíčový důkaz pro třídu *Original*.
- **Inpaintované řasinky (Inpainted):**
  - Pozornost sítě se přesouvá přímo do **míst odstraněných mikrotubulů**.
  - Heatmapy Grad-CAM a Integrated Gradients přesně korelují s ground-truth rozdílovou mapou (sloupec 2).
  - Síť detekuje ztrátu lokální vysokofrekvenční textury, kterou algoritmus Telea nahradil hladkým izotropním přechodem. Model se tak nechová jako "black box", ale činí rozhodnutí na základě skutečných stop po inpaintingu.

---

## 4. Siamské sítě pro měření podobnosti (Image Similarity)

### 4.1 Princip a architektura
Místo přímého zařazení do tříd byla navržena **Siamská síť** (dvě větve se sdílenými vahami), která transformuje snímky do $64$-dimenzionálního embeddingového prostoru:
- **Enkodér:** 3 konvoluční bloky (Conv-BN-ReLU-MaxPool) s postupným růstem kanálů $32 \rightarrow 64 \rightarrow 128$.
- **Projekční hlava:** $\text{Linear}(512 \rightarrow 128) \rightarrow \text{ReLU} \rightarrow \text{Linear}(128 \rightarrow 64)$.
- **L2 normalizace:** Výstupní embedding $z = f_\theta(x)$ je promítnut na jednotkovou hypersféru ($\|z\|_2 = 1$).

### 4.2 Tvorba párů a Contrastive Loss
Byl vygenerován balancovaný dataset párů:
- **Pozitivní páry ($y = 1.0$):** Snímky ze stejné třídy (50 % originál-originál, 50 % inpaint-inpaint).
- **Negativní páry ($y = 0.0$):** Snímky z různých tříd (originál-inpaint).
- **Počet párů:** 1 000 trénovacích párů a 300 validačních párů.

Optimalizace probíhala pomocí **Contrastive Loss** s marginem $m = 1.0$:
$$\mathcal{L}(z_1, z_2, y) = \frac{1}{2} \left[ y \cdot D^2 + (1 - y) \cdot \max(0, m - D)^2 \right]$$
kde $D = \|z_1 - z_2\|_2$ je euklidovská vzdálenost embeddingů. Pro podobné vzorky ($y = 1$) funkce minimalizuje vzdálenost $D^2$, pro odlišné vzorky ($y = 0$) penalizuje vzdálenosti menší než $m$.

### 4.3 Křivka trénování a separace vzdáleností

![](/IMGS3/siamese_results.png)

*Obrázek 6: Průběh trénování siamské sítě (vlevo) a distribuce euklidovských vzdáleností pro pozitivní a negativní validační páry (vpravo).*

- Pozitivní páry dosahují nízkých vzdáleností ($D \approx 0.15 - 0.40$).
- Negativní páry jsou úspěšně vytlačeny k marginu a přesahují jej ($D \approx 0.90 - 1.30$).
- Při rozhodovacím prahu $\tau = 0.5$ dosahuje siamská síť vysoké přesnosti rozlišení shody/neshody tříd.

### 4.4 2D projekce embeddingového prostoru (PCA)

Pro ověření struktury naučeného 64D embeddingového prostoru byla provedena redukce dimenzionality pomocí metody hlavních komponent (PCA) na nezávislých validačních vzorcích:

![](/IMGS3/siamese_embedding_pca.png)

*Obrázek 7: 2D PCA projekce naučeného embeddingového prostoru Siamské sítě. Modré body představují originální řasinky, oranžové body reprezentují inpaintované řasinky.*

V projekci je patrné vytvoření dvou zřetelných, separovaných klastrů odpovídajících oběma třídám. To dokazuje, že kontrastivní učení efektivně uspořádalo reprezentace snímků bez nutnosti klasifikační vrstvy.

### 4.5 Ukázky vyhodnocení konkrétních párů

![](/IMGS3/siamese_pairs_examples.png)

*Obrázek 8: Vizualizace predikcí Siamské sítě na validačních párech – skutečná relace, odhadnutá vzdálenost v prostoru embeddingů a klasifikace shody.*

---

## 5. Závěr a shrnutí

V rámci úlohy 3 byly úspěšně vyřešeny všechny stanovené body:
1. **Dataset a inpainting:** Byl připraven vyvážený dataset 704 vzorků z 352 buněk s bezpečným cell-level splitem eliminujícím data leakage.
2. **Klasifikace:**
   - Vlastní CNN dosáhla solidní přesnosti **~88.7 %**.
   - Předtrénovaný model ResNet18 dosáhl špičkové přesnosti **99.30 %** a F1-skóre **0.993**.
3. **Interpretovatelnost (XAI):** Atribuční mapy (Grad-CAM, Saliency, Integrated Gradients) jednoznačně prokázaly, že se modely při klasifikaci řídí anatomickými strukturami mikrotubulů u originálů a oblastmi lokálního vyhlazení u inpaintovaných buněk.
4. **Siamské sítě:** Pomocí Contrastive Loss byl natrénován model pro měření podobnosti, který dosáhl výborné separace v embeddingovém prostoru, což bylo vizuálně potvrzeno 2D projekcí PCA i histogramy vzdáleností.
