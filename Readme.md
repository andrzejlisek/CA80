# Emulator CA80

## Emulacja

### Sterowanie pracą emulatora

Właściwy emulator znajduje się na zakładce **Emulacja**\. Emulator można uruchomić, zatrzymać i zresetować procesor Z80 \(nie resetuje zawartości pamięci\)\. Bezpośrednio po uruchomieniu emulatora, jest on zatrzymany\.

Uruchamianie i zatrzymanie emulatora jest możliwe za pomocą przycisków **Start** i **Stop** znajdujących się w sekcji **Sterowanie**\. Przycisk **Reset** umożliwia zresetowanie procesora Z80\.

### Wyświetlacz

Obsługiwany jest wyświetlacz siedmiosegmentowy z multipleksacją w sposób uproszczony\. Świecenie każdego segmentu każdego znaku jest utrzymywane przez 65000 taktów zegarowych od ustania sygnału powodującego zaświecenie\.

### Klawiatura

W komputerze CA80 nowej generacji zastosowaną inną konstrukcje układu odczytującego nacisnięcia klawiszy, niekompatybilną z odpowiadającym układem w komputerze CA80 starej generacji\. Emulator obsługuje obie konstrukcje, Stosowaną implementację można wybrać na zakładce **Konfiguracja i pułapki**, poprzez zaznaczenie opcji **Nowy CA80** w sekcji **Ustawienia emulacji**\. Zmiana tego ustawienia jest stosowana natychmiast\.

Klawiaturą można sterować poprzez klikanie przycisków w sekcji **Klawiatura** za pomocą myszki lub za pomocą nacisnięć klawiszy na klawiaturze, lecz aby sterować emulowanym CA80 za pomocą klawiatury, należy myszką kliknąć wyświetlacz\.

### Generator wewnętrzny dźwięku

CA80 posiada zintegrowany generator dźwięku, który jest używany najcześciej jako sygnał naciskania klawiszy\. Generowanie dźwięku polega na wysyłaniu dowolnej wartości na adres \_\_EC \(wartość bardziej znaczącej części adresu nie jest istotna\) z określoną częstotliwością, równą częstotliwości generowanego dźwięku\.

W celu uproszczenia implementacji i zwiększenia wydajności, emulator zapamiętuje momenty dwóch ostatnich pobudzeń generatora przez określoną liczbę cykli zegarowych\. Jeżeli są zapamiętane dwa pobudzenia, to generuje się dźwięk o częstotliwości wynikającej z odstępu czasowego między tymi pobudzeniami, w przypadku braku lub jednego zapamiętanego pobudzenia nie generuje się dźwięku\. W przypadku zapamiętanych dwóch pobudzeń, nowe pobudzenie kasuje informację o starszym z dwóch ostatnich pobudzeń, co powoduje nowe wyliczenie częstotliwości dźwięku\.

Na zakładce **Konfiguracja i pułapki**, w sekcji **Ustawienia generowania dźwięku** mozna ustawić głośność tego dźwięku za pomocą suwaka **Głosność gen\. wew\.**, a wartość w polu **Czas pobudzenia \[clk\]** określa liczbę taktów zegara do utrzymywania stanu pobudzenia generatora\. Zbyt mała wartość może spowodować nieusłyszenie bardzo krótkich efektów dźwiękowych, a zbyt duża spowoduje słyszenie dżwięku przez pewien czas po zakończeniu generowania\.

### Pozytywka

Do CA80 jest przewidziany opcjonalny interfejs pozytywki występujący w dwóch wersjach\. Jest to zewnętrzny generator dźwięku używany zazwyczaj do odtwarzania melodii\. Pierwsza wersja jest przeznaczona do użytku w CA80 bez układu Z80ACTC, a druga do użytku w CA80 z układem Z80ACTC\. Emulator symuluje pierwszą z wymienionych wersji\. Interfejsem steruje się przez port wejścia/wyjścia, linia C0 określa załączenie lub wyłączenie pozytywki \(stan niski okresla załączenie\), a wartości linii B0\-B7 określają częstotliwość dźwięku\. Częstotliwość odtwarzanego dźwieku jest dwukrotnie nizsza, niż wynikająca z dokumentacji CA80 w celu polepszenia jakości, pomimo tego utwory muzyczne są odtwarzane prawidłowo\.

Na zakładce **Konfiguracja i pułapki**, w sekcji **Ustawienia generowania dźwięku**, suwak **Głośność pozytywki** określa głośność generowanego dźwięku\. W przypadku uruchamiania programu, który steruje stanem linii sterujących pozytywką w sytuacji, w której nie przewiduje się użycia pozytywki, głośność pozytywki należy zmniejszyć do minimum w celu uniknięcia usłyszenia nieprzewidzianych dźwięków\.

### Port wejścia/wyjścia

Na zakładce **Emulacja**, w sekcji **Port wejscia/wyjścia** jest zobrazowany port port równoległy wyprowadzony na zewnątrz CA80, który jest używany do komunikacji z urządzeniami zewnętrznymi\. Ten port jest zbudowany w oparciu o układ 8255, w którym sa przewidziane 24 linie w grupach A, B i C\. Każda grupa może być ustawiona jako wejście lub jako wyjscie\. Ponadto, dla grupy C jest możliwe ustawienie, w którym linie C0\-C3 są wejściem, a linie C4\-C7 są wyjściem lub na odwrót\. Kierunek danej linii jest obrazowany przez znak ">" lub "<", który oznacza odpowiednio ustawienie jako wyjscie lub jako wejście\.

W liniach ustawionych jako wyjście, wświetlane zaznaczenie jest odzwierciedleniem stanu linii wymuszanej przez port, a dla linii ustawionych jako wejście można zmieniać stan poprzez zaznaczanie lub odznaczanie danej linii\. Zmiana stanu wywołuje natychmiastowy skutek w przypadku uruchomienia programu reagujacego na zmiany stanu linii wejściowych\.

### Elementy nieobsługiwane

Emulator nie obsługuje układu Z80ACTC, obsługiwane sa układy przewidziane w konstrukcji bez Z80ACTC łącznie z pozytywką\. Urządzenia zewnętrzne podłączane przez port wejścia/wyjścia inne niż pozytywka nie są obsługiwane\.

Procesor Z80 ma określony zestaw zdefiniowanych kodów rozkazów\. Teoretycznie możliwe jest utworzenie kodów rozkazów nieistniejących\. Zachowanie się procesora w przypadku napotkania na kod nieznanego rozkazu nie jest zdefiniowane\. Emulator w takiej sytuacji zatrzyma emulację i zostanie wyświetlony komunikat z informacją o napotkaniu nieistniejącego rozkazu z podaniem kodu potencjalnego rozkazu\. Na ogół przyczyną wystąpienia błędu jest błędna wartość jednego z bajtów w kodzie programu lub próba uruchomienia kodu od adresu, w którym nie ma kodu programu, a są dane\.

### Magnetofon

Emulator umożliwia pracę z magnetofonem, który obsługuje się za pomocą przycisków w sekcji **Magnetofon** na zakładce **Emulacja**\. Najpierw należy wygenerować pustą taśmę za pomocą przycisku **Generuj pustą taśmę**\. Należy podać czas w sekundach\. Nie zaleca się generowania taśmy dłuższej niż 5 minut \(300 sekund\) ze względu na zajmowaną pamięć RAM\. Taśmę można odczytać z pliku lub zapisać do poliku za pomocą przycisków odpowiednio **Wczytaj plik taśmy** i **Zapisz plik taśmy**\.

Taśmę obsługuje się za pomocą pięciu przycisków znajdujących się w górnej części sekcji **Magnetofon** w sposób zbliżony do obsługi prawdziwego magnetofonu\. Są to kolejno **Zatrzymaj**, **Przewiń do przodu**, **Przewiń do tyłu**, **Odtwarzaj**, **Nagrywaj**\. Odtwarzanie, nagrywanie i przewjanie zostanie przerwane w momencie osiągnięcia końca taśmy\. Ponieważ praca magnetofonu jest zsynchronizowana z pracą emulatora, odtwarzanie, nagrywanie i przewijanie jest możliwe wyłącznie podczas pracy emulatora\.

### Ładowanie i zapisywanie zawartości pamięci

Ładowanie i zapisywanie pamięci wykonuje się na zakładce **Pamięć i rejestry**, w sekcji **Pamięć**\.

Aby załadować zawartość z pliku, należy kliknąć przycisk **Wczytaj zawartość**\. W wyświetlonym okienku należy podać adres początkowy ładowanej zawartości w formie czterocyfrowej liczby szesnastkowej, a po zatwierdzeniu wybrać plik\. Liczba zmodyfikowanych bajtów będzie równa wielkości pliku\. Obszary niezapisywalne nie zostaną zmodyfikowane\.

Aby zapisać obszar pamięci, należy kliknąć przycisk **Zapisz zawartość**, a w wyświetlonym okienku podać adres początkowy zapisywanej zawartości w formie czterocyfrowej liczby szesnastkowej, Po zatwierdzeniu, w następnym okienku należy podać adres końcowy w tym samym formacie, a po zatwierdzeniu należy określić nazwę pliku\.

### Zegar czasu rzeczywistego

W przeciwieństwie do oryginalnego CA80 emulator umożliwia dostęp do zegara czasu rzeczywistego w celu pobrania aktualnej daty i godziny\.

Zegar jest dostępny pod adresem \_\_FFh\. Aby odczytać daną część aktualnej daty lub godziny, należy najpierw wysłać pod ten adres numer wartości, a potem odczytać ten adres w celu uzyskania wartości\. Odczytywane wartości są zapisane w standardzie BCD 8421\.

Możliwe jest odczytanie wartości odpowiadających poniższym numerom:  
0\. Dzień tygodnia \(0 \- niedziela, 1 \- poniedziałek itd\.\)  
1\. Dzień miesiąca  
2\. Miesiąc  
3\. Dwie ostatnie cyfry roku  
4\. Dwie pierwsze cyfry roku  
5\. Godzina  
6\. Minuty  
7\. Sekundy  
8\. Setne sekund

Oryginalny program monitora nie przewiduje istnienia zegara czasu rzeczywistego pracującego przez całą dobę, jednakże zapewnia programowy zegar, który należy ustawiać przy kazdoracowym uruchomieniu komputera\. Automatyczna synchronizacja programowego zegara czasu rzeczywistego jest mozliwa za pomocą odpowiednio przygotowanego programu lub zmodyfikowanego programu monitora\.

## Konfiguracja emulatora

### Pamięć RAM i ROM

Rzeczywisty komputer CA80 posiada 4 gniazda pamięci, w których mozna umieszczać pamięć ROM lub RAM w zależności od potrzeb\. Na ogół, w pierwszym gnieździe umieszcza się pamięć ROM z programem monitora, a w czwartym umieszcza się pamięć RAM, ponieważ oryginalny program monitora wydany przez MIK wymaga pamięci RAM w przestrzeni przynajmniej FF00\-FFFF\.

Rodzaj i wielkość poszczególnych obszarów określa się przez w sekcji **Moduły pamięci** na zakładce **Konfiguracja i pułapki**\. W CA80 znajdują się 4 moduły pamięci, każdy obejmuje obszar 16kB\. Zakresy adresów poszczególnych modułów to odpowiednio 0000\-3FFF, 4000\-7FFF, 8000\-BFFF i C000\-FFFF\. Oznaczenia modułów **00**, **40**, **80** i **C0** są wartościami bardziej znaczącego bajtu adresu początkowego, moduł **C0** oznacza moduł obsługujący zakres C000\-FFFF\. W kolumnie **RAM** określa się, czy jest to moduł pamięci RAM \(pamięć zapisywalna\), natomiast w komumnie **Wielkość** określa się wielkość danego modułu\. W przypadku wybrania wielkości mniejszej niż 16kB, ten sam moduł będzie widoczny wielokrotnie w całym zakresie adresów obsługiwanym przez moduł \(adresowanie niepełne\)\. Na przykład, w przypadku wybrania wielkości 4kB dla modułu **40**, ta sama pamięć będzie widoczna czterokrotnie, w zakresach 4000\-4FFF, 5000\-5FFF, 6000\-6FFF i 7000\-7FFF\.

W sekcji **Obrazy pamięci ROM** określa się pliki zawartości, które mają zostać wczytane do pamięci przy starcie eumulatora, bez względu na zapisywalność obszarów pamięci\.

Plik dodaje się za pomocą przycisku **Dodaj plik**\. Należy podać adres początkowy w formie czterocyfrowej liczby szesnastkowej, a następnie wybrać plik\. Adres końcowy zostnie ustalony na podstawie wielkości pliku\. Po dodaniu pliku na listę, zostanie on wczytany natychmiast\.

Wskazany plik na liście można usunąć za pomocą przycisku **Usuń plik**\. Ten przycisk, poza usunięciem pozycji z listy nie wykonuje jakiejkolwiek czynności\.

### Konfiguracja klawiatury

Na zakładce **Konfiguracja i pułapki**, w sekcji **Klawiatura** można modyfikować przypisania klaiszy do przycisków CA80\. Aby przypisać klawisz, na zakładce **Emulacja** należy kliknąć wyświetlacz w sekcji **Wyświetlacz**, nacisnąć klawisz i wrócić do zakładki **Konfiguracja i pułapki**\. W polu **Kod klawisza** w sekcji **Klawiatura** będzie widoczny kod ostatnio wciśniętego klawisza\. W polu **Id klawisza** należy wpisać jednoznakowy identyfikator klawisza CA80\. Dozwolone są cyfry, litery A\-G, M, W\-Z oraz znaki "\." i "="\. Następnie można kliknąć przycisk **Dodaj/Zmień**\. Na liście pojawi się przypisanie kodu klawisza komputera do identyfikatora klawisza CA80\. Aby usunąć przypisanie, należy zaznaczyć je na liście i kliknąć przycisk **Usuń**\.

### Przerwania

Komputer CA80 co 2 milisekundy \(8000 taktów zegara\) wyzwala przerwanie niemaskowalne w procesorze Z80, a interfejs pozytywki wyzwala przerwanie maskowalne co kilkadziesiąt milisekund, częstotliwość wyzwalania ustawia się potencjometrem\.

Na zakładce **Konfiguracja i pułapki**, liczbę taktów zegara pomiędzy dwoma kolejnymi wyzwoleniami przerwania ustawia się poprzez zmianę wartości pola **Okres NMI \[clk\]** i **Okres INT \[clk\]**, odpowiednio dla przerwania niemaskowalnego i przerwania maskowalnego\. Wprowadzenie innej wartości spowoduje zmiane częstotliwości wywoływania przerwania\. Wpisanie liczby 0 lub liczby ujemnej lub odznaczenie pola **Okres NMI \[clk\]** lub **Okres INT \[clk\]** powoduje wyłączenie przerwania \(nie będzie ono wyzwalane\)\.

### Timer interfejsu emulatora

W wątku interfejsu pracuje timer, który zapewnia wyświetlanie bieżącej zawartości wyświetlacza, odtwarzanie dźwięku i innych elementów wyświetlanych w czasie pracy emulatora\. Na zakładce **Konfiguracja i pułapki**, w sekcji **Ustawienia emulacji**, w polu **Interwał timera \[ms\]** można ustawić interwał \(okres\) między dwoma kolejnymi uruchomieniami procedury timera w milisekundach\. Zaleca się wartość pomiędzy 10 a 50\.

### Ustawienia odtwarzania dźwieku

Sposób odtwarzania dźwięku wymusza buforowanie synchronizowane z timerem\. W sekcji **Ustawienia odtwarzania dźwięku** na zakładce **Konfiguracja i pułapki**, pole **Odtwarzanie dźwięku** określa, czy dźwięk jest odtwarzany \(słyszalny\)\. Zmiana zostanie zastosowana po zatrzymaniu i uruchomieniu emulacji lub jej zresetowaniu za pomocą przycisków **Stop** i **Start** lub **Reset **na zakładce **Emulacja**\.

W wymienionej wyżej sekcji ustawia się 2 wartości, które są wyrażone w próbkach dźwięku, dźwięk jest generowany z częstotliwością 48000 próbek na sekundę\. Pole **Długość bufora \[smp\]** określa ilość próbek możliwych do zapisania w urzadzeniu audio, im większa wartość, tym wieksza stabilność dźwięku, ale też dłuższe opóźnienie dźwięku\. Wartość należy dobrać doświadczalnie na danym komputerze, w większości przypadków wartość powinna wynosić pomiędzy 5000 a 10000 próbek\. Pole **Długość elementu \[smp\]** określa długość dźwięku jednorazowo wprowadzaną do bufora przy wyzwoleniu timera\. Wartość należy dobrać doświadczalnie, powinna być nieco większa o ok\. 50% niż liczba próbek wynikająca z interwału timera\. Na przykład dla interwału 20 milisekund teoretycznie wynosi 960 próbek, ale optymalna wartość wynosi ok\. 1500 próbek\.

## Pamięć, rejestry i śledzenie pracy

### Podgląd pamięci i rejestrów

Na zakładce **Pamięć i rejestry** możliwy jest podgląd zawartości pamięci oraz rejestrów procesora Z80\. Aby podejrzeć zawartość pamięci lub wartości rejestrów, w sekcji **Sterowanie** należy zaznaczyć **Odświeżaj**, zawartość będzie odświeżana na bieżąco\. W tytule sekcji **Pamięć** widoczny jest aktualnie podglądany zakres\. Aby zmienić zakres, należy przesunąć suwak znajdujący się poniżej pola wyświetlającego zawartość\. W sekcji **Pamięć** widocznych jest 16 bajtów począwszy od rejestru PC, co umożliwia odczyt najbliższych rozkazów, które zostaną wykonane oraz 16 bajtów począwszy od rejestru SP, co umożliwia podejrzenie zawartości stosu\. Odznaczenie pola **Odświeżaj** w sekcji **Sterowanie** spowoduje zaprzestanie odświeżania zawartości pamięci i wartości rejestrów i tym samym utrwalenie ostatniego odczytu\.

### Pułapki i praca krokowa

Na zakładce **Konfiguracja i pułapki**, w sekcji **Pułapki** możliwe jest definiowanie pułapek, czyli adresów zatrzymania emulatora\. Podczas pracy emulatora, w momencie osiągnięcia adresu jednej z pułapek, emulacja zostanie zatrzymana i zostanie wyswietlony stosowny komunikat\.

Aby dodać pułapkę, należy kliknąć przycisk **Dodaj** i podać adres pułapki w formie czterocyfrowej liczby szesnastkowej\. W celu usunięcia pułapki, należy zaznaczyć jej adres na liście i kliknąć przycisk **Usuń**\.

Na zakładce **Pamięć i rejestry**, w sekcji **Sterowanie**, znajdują się przyciski **Start**, **Stop** i **Krok**\. Przyciski **Start** i **Stop** powodują odpowiednio uruchomienie i zatrzymanie pracy emulatora\.

Przycisk **Krok** powoduje wykonanie jednego rozkazu\. W tym celu emulacja musi być zatrzymana\. Każde kliknięcie przycisku **Krok** spowoduje wykonanie jednego rozkazu\. Dzięki temu można obserwować zmiany w zawartości pamięci i zmiany wartości rejestrów w wyniku wykonania danego rozkazu\. Przy pracy krokowej, w przypadku napotkania pułapki, komunikat o tym fakcie nie zostanie wyświetlony\.




