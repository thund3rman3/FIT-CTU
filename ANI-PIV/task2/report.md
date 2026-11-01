# Úkol 2: Segmentace mikrotubulů pomocí U-Net



## 1. Příprava datasetu a předzpracování

Pro trénování byl vytvořen vlastní Dataset postavený na PyTorch.

### 1.1 Načtení dat a rozdělení (Train / Validation Split)
* **Vstupní data:** Celkem nalezeno **352 párů** snímků buněk a binárních masek (`Data/cells/` a `Data/masks_pts/`).
* **Rozdělení sady:** Dataset je rozdělen v poměru **80 : 20**:
  * **Trénovací sada:** 281 snímků (80 %)
  * **Validační sada:** 71 snímků (20 %)
 
* **DataLoader:** Velikost dávky (batch size) byla nastavena na **8**. Trénovací data jsou náhodně míchána (`shuffle=True`), validační nikoliv.

### 1.2 Předzpracování a normalizace
* **Formát vstupů:** Vstupní snímky i masky se načítají v odstínech šedi (`cv2.IMREAD_GRAYSCALE`).
* **Binarizace masky:** maska je binarizována podmínkou `mask > 127` ($0.0$ pro pozadí, $1.0$ pro mikrotubuly/řasinky).
* **Změna rozměrů a typů:** Pomocí `v2.Resize` jsou snímky a masky sjednoceny na rozlišení **$256 \times 256$ px** (bilineární interpolace). Data jsou převedena a normalizována do intervalu $[0.0, 1.0]$.


### 1.3 Augmentace dat
* **Náhodné horizontální převrácení:** $p = 0.5$.
* **Náhodné vertikální převrácení:** $p = 0.5$.

---

## 2. Architektura modelu U-Net

Použitá síť implementuje plně konvoluční architekturu **U-Net** s enkodérem, dekodérem a skip connections, upravenou pro rychlou konvergenci na obrázcích velikosti $256 \times 256$ .

### 2.1 DoubleConv
Základním stavebním kamenem sítě je blok složený ze dvou konvolučních vrstev:
$$\text{Conv2D}(k \times k, \text{pad}=\lfloor k/2 \rfloor) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU} \rightarrow \text{Conv2D}(k \times k, \text{pad}=\lfloor k/2 \rfloor) \rightarrow \text{BatchNorm2D} \rightarrow \text{ReLU}$$
* **Padding:** Hodnota $\lfloor k/2 \rfloor$ (pro jádro $3 \times 3$ padding = 1) zachovává prostorové rozměry příznakových map uvnitř bloku.
* **Batch Normalization:** Stabilizuje distribuci aktivací napříč vrstvami, zrychluje trénink a působí jako mírná regularizace.

### 2.2 Architektura (default depth = 3)
1. **Enkodér (Downsampling):**
   * **1. úroveň (`enc1`):** Vstup 1 kanál $\rightarrow$ 32 kanálů, rozlišení $256 \times 256$.
   * `MaxPool2d(2)` zmenší rozlišení na $128 \times 128$.
   * **2. úroveň (`enc2`):** 32 kanálů $\rightarrow$ 64 kanálů, rozlišení $128 \times 128$.
   * `MaxPool2d(2)` zmenší rozlišení na $64 \times 64$.
   * **3. úroveň (`enc3`):** 64 kanálů $\rightarrow$ 128 kanálů, rozlišení $64 \times 64$.
   * `MaxPool2d(2)` zmenší rozlišení na $32 \times 32$.
   * **Bottleneck:** 128 kanálů $\rightarrow$ 256 kanálů, rozlišení $32 \times 32$.
2. **Dekodér (Upsampling) a Skip Connections:**
   * **Zvětšení rozlišení:** Realizováno pomocí bilineárního převzorkování `nn.Upsample(scale_factor=2, mode='bilinear')`.
   * **Skip Connections (`torch.cat`):** Příznakové mapy z odpovídající úrovně enkodéru se zřetězí s převzorkovaným výstupem dekodéru podél dimenze kanálů, čímž se do dekodéru vrací jemná prostorová informace o tenkých vláknech ztracená při poolingu.
   * **1. dekódovací úroveň (`dec2`):** Spojení $256 + 128 = 384$ kanálů $\rightarrow$ redukce na 128 kanálů.
   * **2. dekódovací úroveň (`dec1`):** Spojení $128 + 64 = 192$ kanálů $\rightarrow$ redukce na 64 kanálů.
   * **3. dekódovací úroveň (`dec0`):** Spojení $64 + 32 = 96$ kanálů $\rightarrow$ redukce na 32 kanálů při původním rozlišení $256 \times 256$.
3. **Výstupní vrstva (`out_conv`):**
   * Konvoluce $1 \times 1$ převede 32 kanálů na 1 výstupní kanál s raw logity

---

## 3. Trénování modelu a experimenty se ztrátovými funkcemi

### 3.1 Nastavení trénovací smyčky
* **Optimalizátor:** Adam (`learning rate = 0.001`).
* **Počet epoch:** 10 epoch pro každý experiment.
* **Sledované metriky:** Během každé epochy byla počítána hodnota ztrátové funkce na trénovací sadě (`Train Loss`) a na validační sadě (`Val Loss`), doplněná o validační `Dice Score`, `Accuracy`, `Precision`, `Recall` a `F1-Score`.

### 3.2 Kvantitativní srovnání ztrátových funkcí (10. epocha)

| Ztrátová funkce | Train Loss | Val Loss | Dice Score | Accuracy | Precision | Recall | F1-Score |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **BCE** | 0.0721 | 0.0728 | **0.8092** | 0.9744 | 0.7511 | 0.8868 | **0.8127** |
| **BCE + Dice ($\alpha=0.5$)** | 0.1792 | 0.1625 | 0.8040 | 0.9712 | 0.7019 | 0.9481 | 0.8055 |
| **Pure Dice Loss** | 0.2437 | 0.2392 | 0.7837 | 0.9712 | 0.7359 | 0.8471 | 0.7866 |
| **Focal Loss ($\alpha=0.25, \gamma=2.0$)** | 0.0043 | 0.0051 | 0.6489 | 0.9639 | **0.8251** | 0.5454 | 0.6545 |
| **Focal Loss ($\alpha=0.70, \gamma=1.0$)** | 0.0223 | 0.0232 | 0.8037 | 0.9730 | 0.7250 | 0.9171 | 0.8090 |
| **Tversky Loss ($\alpha=0.7, \beta=0.3$)** | 0.2224 | 0.2057 | 0.7478 | 0.9586 | 0.6042 | **0.9887** | 0.7496 |

### 3.3 Analýza a výběr ztrátové funkce
* **BCE:** Dosáhla celkově nejvyššího **Dice skóre (0.8092)** i **F1-skóre (0.8127)**. Poskytuje nejvyváženější poměr mezi Precision (75.11 %) a Recall (88.68 %). Plynulý gradient binární křížové entropie vedl k nejstabilnější konvergenci bez oscilací. Byla proto zvolena jako **hlavní referenční funkce pro další ladění**.

![](/IMGS2/BCE/D4g3.png)

![](/IMGS2/BCE/D4epochs.png)

* **BCE + Dice:** Přinesla vysoký Recall (94.81 %) i Dice (0.8040), avšak s mírným nárůstem false-positive pixelů na okrajích vláken (Precision klesla na 70.19 %).
![](/IMGS2/BCE+Dice/g3.png)
![](/IMGS2/BCE+Dice/epochs2.png)


* **Pure Dice Loss:** Přímo optimalizuje překryv oblastí, ale v úvodních epochách vykazovala pomalejší a méně stabilní gradienty než BCE.
![](/IMGS2/DiceLoss/g3.png)
![](/IMGS2/DiceLoss/epochs2.png)


* **Focal Loss ($\alpha=0.25, \gamma=2.0$ vs. $\alpha=0.70, \gamma=1.0$):** Standardní nastavení ($\alpha=0.25$) potlačilo pozitivní třídu (Recall pouhých 54.54 %, Dice 0.6489).
![](/IMGS2/focalLoss/g3.png)
![](/IMGS2/focalLoss/epoch2.png)


Přenastavením vah na $\alpha=0.70$ a $\gamma=1.0$ došlo k dramatickému nárůstu Recallu na 91.71 % a Dice na 0.8037, což potvrdilo klíčový vliv váhy tříd u tenkých struktur.


* **Tversky Loss:** Zvýšením váhy pro False Negatives ($\alpha=0.7$) dosáhla extrémního Recallu (98.87 %), avšak za cenu silného šumu v pozadí (Precision pouze 60.42 %).
![](/IMGS2/tversky/g3.png)
![](/IMGS2/tversky/epoch2.png)

---

## 4. Vyhodnocení modelu

### 4.1 Kvalitativní vyhodnocení
* **Vývoj predikcí v čase:** V 1. epoše model predikuje celou plochu jako pozadí (Dice = 0.0000). Již ve 2. epoše síť lokalizuje hlavní tubuly a od 4. epochy jsou predikované kontury hladké a věrně kopírují ground truth.
* **Typické chybové stavy (Failure Cases):**
  1. **Falešná pozitiva (FP):** Model občas detekuje membrány, nebo fázový šum na pozadí, které mají v mikroskopu podobný jasový kontrast jako mikrotubuly. V okolí silných vláken dochází k mírnému "rozpíjení" masky.
  2. **Falešná negativa (FN):** Přerušená nebo nedetekovaná velmi tenká, nízko-kontrastní vlákna a koncové výběžky řasinek.
* **Důvody chyb:**
  * Buňky obsahují šum a nerovnoměrné nasvícení.
  * Chybovost ground truth segmentacních masek

### 4.2 Kvantitativní vyhodnocení 
Na celé validační sadě (71 snímků o rozměru $256 \times 256$ px, celkem **4 653 056 pixelů**) byly pro finální model BCE (hloubka 3, jádro 3) sečteny jednotlivé stavy konfuzní matice:

| Metrika / Parametr | Hodnota | Procentuální podíl |
| :--- | :---: | :---: |
| **True Negatives (TN)** | **4 252 131** px | **91.38 %** |
| **True Positives (TP)** | **275 827** px | **5.93 %** |
| **False Positives (FP)** | **109 499** px | **2.35 %** |
| **False Negatives (FN)** | **15 599** px | **0.34 %** |
| **Accuracy** | **0.9731** (97.31 %) | — |
| **Precision** | **0.7158** (71.58 %) | — |
| **Recall** | **0.9465** (94.65 %) | — |
| **F1-Score** | **0.8151** | — |
| **Dice Coefficient** | **0.8151** | — |

* **Nevhodnost metriky Accuracy:** Pozadí tvoří **93.74 %** všech pixelů v datasetu, zatímco mikrotubuly pouze **6.26 %**. Triviální klasifikátor predikující samé nuly dosahuje Accuracy 93.73 % při Dice = 0. Klíčovými metrikami jsou proto **Dice score** a **F1-score**, které měří skutečný prostorový překryv zájmových vláken.
* **Konfuzní matice:** 
![](/IMGS2/BCE/confusion_matrix.png)


---

## 5. Ladění hyperparametrů

Experimenty byly provedeny s nejlépe fungující ztrátovou funkcí **BCE** za identických trénovacích podmínek (10 epoch, lr = 0.001, batch size = 8).

### 5.1 Vliv hloubky sítě U-Net při kernel size = 3

| Hloubka sítě | Parametry / Úrovně | Val Loss (10. ep.) | Dice Score | Accuracy | Precision | Recall | F1-Score |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Hloubka 2 (mělčí)** | 2 úrovně ($256 \rightarrow 128$) | 0.1206 | 0.7154 | 0.9496 | 0.5615 | **0.9860** | 0.7138 |
| **Hloubka 3 (baseline)** | 3 úrovně ($256 \rightarrow 128 \rightarrow 64$) | **0.0728** | **0.8092** | 0.9744 | 0.7511 | 0.8868 | **0.8127** |
| **Hloubka 4 (hlubší)** | 4 úrovně ($256 \rightarrow 128 \rightarrow 64 \rightarrow 32$) | 0.0696 | 0.7727 | **0.9765** | **0.9162** | 0.6849 | 0.7832 |

#### Analýza vlivu hloubky:
1. **Příliš mělká síť (Hloubka 2):**
   * Síť nemá dostatečný globální kontext buňky k odlišení pozadí od shluků mikrotubulů.
   * Důsledkem je extrémně nízká **Precision (56.15 %)** a nestabilita v tréninku (např. v 9. epoše došlo k propadu Dice na 0.2401). Síť sice detekuje téměř všechna vlákna (Recall 98.60 %), ale za cenu masivního množství falešně pozitivních pixelů na pozadí.
2. **Příliš hluboká síť (Hloubka 4):**
   * Čtyřnásobný pooling zmenšuje prostorové rozlišení v bottlenecku až na $16 \times 16$ px. Pro jemné jednopixelové mikrotubuly to znamená nevratnou ztrátu prostorové lokalizace.
   * Model má vzhledem k malému trénovacímu datasetu (281 snímků) příliš mnoho parametrů. Síť začne být příliš konzervativní – dosahuje výborné **Precision (91.62 %)**, ale dramaticky jí klesá **Recall (68.49 %)** a celkové **Dice skóre (0.7727)**, protože tenká a méně kontrastní vlákna zcela vynechává.
3. **Která hloubka fungovala nejlépe a proč:**
   * Nejlépe fungovala **Hloubka 3**. Představuje optimální kompromis: receptivní pole je dostatečně velké pro zachycení celého kontextu buňky, zmenšení na $32 \times 32$ px v bottlenecku ještě nemaže prostorovou strukturu vláken a kapacita modelu odpovídá velikosti trénovacího datasetu bez sklonu k přetrénování.

---

### 5.2 Vliv velikosti konvolučního jádra při hloubce 3

| Velikost jádra | Val Loss (10. ep.) | Dice Score | Accuracy | Precision | Recall | F1-Score |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Kernel $1 \times 1$** | 0.0803 | 0.7555 | 0.9722 | **0.8064** | 0.7450 | 0.7675 |
| **Kernel $3 \times 3$** | 0.0728 | 0.8092 | 0.9744 | 0.7511 | 0.8868 | 0.8127 |
| **Kernel $5 \times 5$** | **0.0626** | **0.8357** | **0.9783** | 0.7766 | **0.9185** | **0.8406** |

#### Analýza velikosti jádra:
1. **Jádro $1 \times 1$:** Konvoluce neintegruje žádné prostorové okolí pixelu a funguje pouze jako lineární projekce kanálů. Jediné prostorové sdružování probíhá přes pooling. Pro segmentaci spojitých prostorových křivek je toto jádro nedostačující (Dice 0.7555, Recall 74.50 %).
2. **Jádro $3 \times 3$:** Standardní a výpočetně efektivní velikost. Zachycuje lokální gradienty a orientaci vláken, dosahuje vyváženého poměru Precision i Recallu (Dice 0.8092).
3. **Jádro $5 \times 5$:** Větší receptivní pole v každé konvoluci lépe navazuje sousední segmenty tenkých mikrotubulů přes větší vzdálenost. Dosáhlo nejvyššího **Dice skóre (0.8357)** i nejnižší validační ztráty (**0.0626**), vyžaduje však vyšší výpočetní nároky.

---

## 6. Souhrnný závěr

1. **Dataset a metrika:** Vzhledem k nerovnováze tříd (93.74 % pozadí vs. 6.26 % mikrotubulů) je metrika Accuracy zavádějící. Hlavními hodnotícími ukazateli jsou **Dice koeficient** a **F1-skóre**.
2. **Ztrátová funkce:** Nejvyváženější výsledky poskytla **Binary Cross-Entropy (`BCEWithLogitsLoss`)** s Dice 0.8092 a F1 0.8127. U Focal Loss bylo nutné výrazně zvýšit váhu pozitivní třídy ($\alpha=0.7$), jinak síť ignorovala tenká vlákna.
3. **Architektura U-Net:**
   * **Hloubka 3** se ukázala jako ideální volba pro rozlišení $256 \times 256$ a danou velikost datasetu. Mělčí síť (hloubka 2) trpí nedostatečným kontextem a šumem, hlubší síť (hloubka 4) ztrácí prostorové rozlišení vláken v bottlenecku.
   * Zvětšení jádra z **$3 \times 3$ na $5 \times 5$** přineslo další znatelné zlepšení Dice skóre na **0.8357**, protože větší konvoluční okno lépe sleduje návaznost podlouhlých biologických struktur.
