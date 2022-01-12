NOTE: Currently this program does NOT correctly fail if there is no internet connection. This is due to IBM's lib not returning correct codes.



Pirmoji užduotis: RUTX gaminio prijungimas prie IBM Watson debesies
Su šia užduotimi yra siekiama įtvirtinti pirmasias įgytas teorinias ir praktinias žinias kaip OpenWRT sistemoje yra kuriami paketai, bei išmokti pritaikyti trečiosios šalies teikiamą programinę įrangą.

Užduoties metu bus sukurta programa, kuri į IBM Watson debesį siųs pasirinktus duomenis iš router’io.

Prisijungimą prie IBM Watson sistemos galima rasti čia. Prisijungus prie sistemos, reikia susirasti Internet of things platform servisą ir pasirinkti nemoką planą. Nesumaišykite su Internet of things platform-h3 servisu.

Svarbu! Prieš pradedant atlikti užduotį, svarbu susirasti ir pritaikyti OpenWRT sistemai biblioteką/SDK, kuri bus naudojama kuriamoje programoje prisijungimui prie IBM Watson debesies. Prisijungti prie debesies galimi ir kiti variantai, bet šiuo atveju reikia susirasti jau sukurtą biblioteką/SDK ir mokėti ją pritaikyti OpenWRT sistemai.

Radus biblioteką/SDK, būtina ją pasitvirtinti su praktikos vadovu.

Užduočiai atlikti skiriama pusantros savaitės.

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Užduoties rezultatas:

Paruošta daemon tipo programa, kuri siųs duomenis į IBM Watson teikiamą Internet of things platform servisą.

Surasta biblioteka arba SDK, kuri bus naudojama komunikacijai su IBM watson debesimi sukurtoje daemon programoje. Biblioteka/SDK paruošta kaip atskiras paketas

Paruošta web sąsaja, per kurią bus įjungiama/išjungiama programa ir pateikti tik reikalingiausi įvesties laukai skirti sukonfigūruoti programą

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Viso turi būti paruošti trys skirtingi paketai:

Daemon tipo programa atsakinga už komunikaciją su IBM Watson

Biblioteka/SDK, kuri bus naudojama komunikacijai ir duomenų siuntimu į IBM Watson

WEB sąsaja

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Reikalavimai užduočiai:

Bibliotekos/SDK kodas turi būti automatiškai parsiunčiamas iš nuotolinio serverio, kuriame buvo rasta ši biblioteka/SDK. OpenWRT sistemoje, package kataloge, neturi būti šios bibliotekos/SDK kodo. Kodas nėra laikomas Makefile, shell ir lua skriptai, patch’ai konfigūracijos failai.

Daemon programoje, duomenų gavimui apie router’į, turi būti naudojama ubus sistema. Kokie duomenys bus siunčiami į IBM Watson nėra svarbu. Galima siųsti tiesiog gautus duomenis apie router’yje sunaudojamą kiekį RAM atminties.

Daemon programoje, duomenų perdavimui tarp metodų, neturi būti globalių kintamųjų.

Daemon programa būtinai turi vesti pranešimus į log sistemą, kurios pranešimus galima peržiūrėti router’io komandinėje eilutėje įvykdžius logread komandą. Pranešimai turi būti aiškūs ir informatyvus, leidžiantys vartotojui suprasti apie programos sėkmingą ir klaidingą veikimą.

WEB sąsajos puslapis turi būti tarp Services skilties pasirinkimų.
