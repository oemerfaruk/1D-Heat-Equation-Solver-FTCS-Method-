#include "HeatSolver.hpp"
#include <iostream>
#include <stdexcept>

int main()
{
    // ── Simülasyon parametreleri ──────────────────────────────────────
    const int    nodes      = 50;
    const double alpha      = 0.01;  // Çelik için tipik ısı yayılımı [m²/s]
    const double dx         = 0.1;   // Uzay adımı [m]
    const double dt         = 0.02;  // Zaman adımı [s]  →  r = 0.02 <= 0.5 ✓
    const int    time_steps = 500;

    const double left_bc    = 100.0; // Sol uç sıcaklığı  [°C]
    const double right_bc   =   0.0; // Sağ uç sıcaklığı  [°C]
    const double init_temp  =  20.0; // İlk iç sıcaklık   [°C]
    const std::string output_file = "output.csv";
    // ─────────────────────────────────────────────────────────────────

    try {
        // 1. Çözücüyü oluştur — constructor kararlılık şartını kontrol eder.
        HeatSolver solver(nodes, alpha, dx, dt);

        std::cout << "Kararlilik katsayisi r = "
                  << solver.getStabilityCoefficient()
                  << " (<= 0.5 olmali)\n";

        // 2. Önce sınır koşulları, sonra başlangıç koşulu (sıra önemli!).
        solver.setBoundaryConditions(left_bc, right_bc);
        solver.setInitialCondition(init_temp);

        // 3. Zaman döngüsü
        std::cout << "Simulasyon basliyor (" << time_steps << " adim)...\n";
        for (int t = 0; t < time_steps; ++t)
            solver.step();

        // 4. Sonuçları kaydet
        solver.saveToCSV(output_file);
        std::cout << "Simulasyon tamamlandi. Sonuclar '"
                  << output_file << "' dosyasina kaydedildi.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[HATA] " << e.what() << "\n";
        return 1;
    }

    return 0;
}