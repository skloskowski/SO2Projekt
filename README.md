## Problem jedzących filozofów

Problem jedzących filozofów to klasyczny problem synchronizacji współbieżnych zasobów. Został on zaproponowany przez Edsgera Dijkstrę - holenderskiego pioniera informatyki. Problem ma na celu zobrazowanie wyzwań związanych z zarządzeniem zasobami w systemach wielozadaniowych.

### Opis problemu

Przy okrągłym stole siedzi określona liczba filozofów - w programie podawana jako argument, jednak nie mniejsza niż 2. Filozofowie mają przed sobą talerze z jedzeniem i do ich spożycia potrzebują sztućców. 
Pomiędzy każdym z filozofów znajduje się jeden widelec, który w celu prawidłowego działania programu musi być współdzielony w odpowiednich momentach pomiędzy nimi.

Każdy z filozofów znajduje się w jednym z trzech stanów:

- Myślenie - filozof w obecnym stanie nie wymaga żadnego zasobu.
- Chęć jedzenia - filozof próbuje uzyskać dostęp do wymaganych dla niego zasobów - w tym przypadku potrzebuje obu sąsiadujących widelców.
- Jedzenie - filozof kiedy ma do dyspozycji oba sąsiadujące widelce jest w stanie przez jakiś czas jeść.

### Race condition i deadlock

Jednym z głównych problemów, który może powstać jest zakleszczenie (ang. deadlock). Powstaje on w sytuacji, kiedy każdy z filozofów podniesie jeden z widelców, wtedy żaden z nich nie będzie w stanie wykonać swojego zadania.
W celu rozwiązania tego problemu zaimplementowano asymetryczne blokowanie widelców. Filozofowie o parzystych numerach najpierw podnoszą lewy widelec, a potem prawy, natomiast filozofowie o nieparzystych numerach podnoszą najpierw prawy widelec, a dopiero potem lewy.
Dzięki temu podejściu unika się sytuacji, w której wszyscy filozofowie jednocześnie podnoszą tylko jeden widelec i blokują się nawzajem. W klasycznym przypadku, gdy każdy filozof zaczyna od podniesienia tego samego widelca (np. najpierw lewego), 
może dojść do sytuacji, w której żaden z nich nie będzie mógł zdobyć drugiego widelca i program zatrzyma się w zakleszczeniu. Asymetryczne podejście sprawia, że przynajmniej jeden z filozofów w danym momencie może zdobyć dwa widelce i rozpocząć jedzenie, a po jego zakończeniu zwróci je do użycia przez innych filozofów.

Pierwszym miejscem, gdzie zastosowano synchronizację, jest dostęp do widelców. Każdy filozof potrzebuje dwóch widelców, aby móc jeść, a każdy widelec jest reprezentowany przez oddzielny mutex. Gdy filozof chce rozpocząć jedzenie,
blokuje dwa mutexy – najpierw ten odpowiadający jego numerowi, a następnie ten należący do sąsiada. Dzięki temu zmniejsza się ryzyko sytuacji, w której wszyscy filozofowie jednocześnie podniosą jeden widelec i będą czekać na drugi, blokując siebie nawzajem. 
Po zakończeniu jedzenia filozof zwalnia oba mutexy, co pozwala innym wątkom na kontynuowanie działania.

Drugim miejscem, w którym rozwiązano problem sekcji krytycznej, jest wypisywanie komunikatów na ekran. Ponieważ std::cout nie jest thread-safe, wielu filozofów mogłoby jednocześnie próbować wyświetlić tekst, powodując jego przeplatanie i utratę czytelności. Aby temu zapobiec, 
przed każdą operacją wypisywania blokowany jest mutex outputMutex. Dzięki temu tylko jeden filozof w danym momencie ma dostęp do cout, co gwarantuje poprawne wyświetlanie komunikatów.
Dodatkowo użycie lock_guard<mutex> zapewnia automatyczne zwalnianie mutexa po opuszczeniu sekcji krytycznej, eliminując ryzyko deadlocku wynikającego z zapomnienia o jego odblokowaniu.

### Uruchamianie programu

Żeby uruchomić program należy go najpierw sklonować z repozytorium oraz skompilować, udostępniony został folder build zbudowany przy użyciu Cmake (dostępny do użycia na systemie Windows).

``` sh
git clone https://github.com/skloskowski/SO2Projekt
cd SO2Projekt
cd build
```

W zależności od zainstalownej wersji kompilatora należy wprowadzić jeden z poniższych komend:

```sh
make
```

```sh
mingw32-make
```

Po weryfikacji, że plik został skompilowany poprawnie należy użyć komendy:

```sh
.\SO2Projekt <liczba filozofów>
```

W celu poprawnego działania programu liczba filozofów nie powinna być mniejsza niż 3
