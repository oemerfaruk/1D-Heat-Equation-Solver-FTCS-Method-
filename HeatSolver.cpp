#include "HeatSolver.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

// ─────────────────────────────────────────────
// Constructor: parametre doğrulama + kararlılık kontrolü
// ─────────────────────────────────────────────
HeatSolver::HeatSolver(int grid_points, double diffusivity,
                        double space_step,  double time_step)
    : nx(grid_points), alpha(diffusivity), dx(space_step), dt(time_step)
{
    // --- Parametre doğrulama ---
    if (nx <= 2)
        throw std::invalid_argument("grid_points en az 3 olmalidir (2 sinir + 1 ic nokta).");
    if (alpha <= 0.0)
        throw std::invalid_argument("Isi yayinim katsayisi (alpha) pozitif olmalidir.");
    if (dx <= 0.0)
        throw std::invalid_argument("Uzay adimi (dx) pozitif olmalidir.");
    if (dt <= 0.0)
        throw std::invalid_argument("Zaman adimi (dt) pozitif olmalidir.");

    // --- Von Neumann kararlılık şartı: r = alpha*dt/dx² <= 0.5 ---
    r = alpha * dt / (dx * dx);
    if (r > 0.5)
        throw std::runtime_error(
            "Von Neumann kararlilik sarti ihlal edildi: r = " + std::to_string(r) +
            " > 0.5. Lutfen dt kucultun veya dx buyutun.");

    u.resize(nx, 0.0);
    u_next.resize(nx, 0.0);
}

// ─────────────────────────────────────────────
// Başlangıç koşulu: iç noktaları doldur
// ─────────────────────────────────────────────
void HeatSolver::setInitialCondition(double internal_temp)
{
    // Sınır noktaları (0 ve nx-1) setBoundaryConditions() tarafından yönetilir.
    for (int i = 1; i < nx - 1; ++i)
        u[i] = internal_temp;
}

// ─────────────────────────────────────────────
// Sabit Dirichlet sınır koşulları
// ─────────────────────────────────────────────
void HeatSolver::setBoundaryConditions(double left_temp, double right_temp)
{
    // Her iki tampona da uygulanır; step() boyunca değişmeden kalır.
    u[0]      = left_temp;   u_next[0]      = left_temp;
    u[nx - 1] = right_temp;  u_next[nx - 1] = right_temp;
}

// ─────────────────────────────────────────────
// FTCS adımı — O(1) tampon takası (swap)
// ─────────────────────────────────────────────
void HeatSolver::step()
{
    // İç noktalar için sonlu fark denklemi:
    //   u_next[i] = u[i] + r * (u[i+1] - 2*u[i] + u[i-1])
    for (int i = 1; i < nx - 1; ++i)
        u_next[i] = u[i] + r * (u[i + 1] - 2.0 * u[i] + u[i - 1]);

    // std::swap: sadece dahili pointer'ları değiştirir — O(1), bellek kopyası yok.
    // std::move'un aksine, her iki vektör de swap sonrası tamamen geçerlidir.
    std::swap(u, u_next);

    // Sınır koşullarını koruyun (swap sonrası u_next artık eski u'dur;
    // iç noktaları bir sonraki adımda yeniden yazılacak, sınırlar sabit kalmalı).
    u_next[0]      = u[0];
    u_next[nx - 1] = u[nx - 1];
}

// ─────────────────────────────────────────────
// CSV çıktısı: x koordinatı, sıcaklık
// ─────────────────────────────────────────────
void HeatSolver::saveToCSV(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("CSV dosyasi acilamadi: " + filename);

    file << "x,temperature\n"; // başlık satırı
    for (int i = 0; i < nx; ++i)
        file << i * dx << "," << u[i] << "\n";

    // ofstream destructor'ı dosyayı kapatır; açık kalma riski yok.
}