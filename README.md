# 1D Heat Equation Solver (FTCS Method)

Sonlu Farklar Yöntemi (FDM) ile Modern C++ implementasyonu — **FTCS (Forward-Time Central-Space)** şeması.

---

## Matematiksel Arka Plan

### Yönetici Denklem

1D geçici ısı iletimi denklemi (parabolik PDE):

$$\frac{\partial u}{\partial t} = \alpha \frac{\partial^2 u}{\partial x^2}$$

| Sembol | Anlam | Birim |
|--------|-------|-------|
| $u(x,t)$ | Sıcaklık alanı | °C |
| $\alpha$ | Isı yayınım katsayısı | m²/s |
| $x$ | Uzay koordinatı | m |
| $t$ | Zaman | s |

### FTCS Ayrıklaştırması

İleri fark (zamanda) + merkezi fark (uzayda):

$$u_i^{n+1} = u_i^n + \underbrace{\frac{\alpha \Delta t}{\Delta x^2}}_{r} \left(u_{i+1}^n - 2u_i^n + u_{i-1}^n\right)$$

### Sayısal Kararlılık — Von Neumann Analizi

FTCS **koşullu kararlı**dır. Kararlılık şartı:

$$r = \frac{\alpha \,\Delta t}{\Delta x^2} \leq 0.5$$

> ⚠️ Bu şart ihlal edilirse çözücü **constructor'da exception fırlatır**.

---

## Proje Yapısı

```
.
├── HeatSolver.hpp   # Sınıf arayüzü (API + Doxygen belgeleri)
├── HeatSolver.cpp   # FTCS implementasyonu
├── main.cpp         # Örnek sürücü programı
└── README.md
```

---

## Derleme

```bash
# g++ (C++17)
g++ -std=c++17 -O2 -Wall -Wextra -o heat_solver main.cpp HeatSolver.cpp

# CMake kullanıyorsanız
cmake -B build && cmake --build build
```

---

## Kullanım

```cpp
// 1. Çözücüyü oluştur (kararlılık otomatik kontrol edilir)
HeatSolver solver(/*grid_points=*/50, /*alpha=*/0.01,
                  /*dx=*/0.1, /*dt=*/0.02);

// 2. Önce sınır, sonra başlangıç koşulu (sıra önemli!)
solver.setBoundaryConditions(/*left=*/100.0, /*right=*/0.0);
solver.setInitialCondition(/*T0=*/20.0);

// 3. Zaman ilerletme
for (int t = 0; t < 500; ++t)
    solver.step();

// 4. Sonuçları kaydet
solver.saveToCSV("output.csv");
```

### Çıktı Formatı (`output.csv`)

```
x,temperature
0.0,100.0
0.1,97.3
...
4.9,0.0
```

---


## Performans Notları

- **Ardışık bellek:** `std::vector<double>` CPU önbellek dostu veri düzeni sağlar.
- **Sıfır kopyalı zaman adımı:** `std::swap` yalnızca dahili pointer'ları değiştirir — O(1), heap kopyası yok.
- **Önceden hesaplama:** `r` katsayısı constructor'da bir kez hesaplanır; döngüde yeniden hesaplanmaz.

---

## Sınırlamalar

- Yalnızca **1D** geometri desteklenmektedir.
- **Dirichlet** sınır koşulları (sabit uç sıcaklıkları); Neumann veya Robin desteklenmez.
- FTCS **explicit** şeması olduğundan `r <= 0.5` şartı sıkı biçimde uygulanır.  
  Daha büyük zaman adımları için **Crank–Nicolson** (implicit) şeması tercih edilmelidir.
# 1D-Heat-Equation-Solver-FTCS-Method-
