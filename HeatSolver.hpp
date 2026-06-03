#pragma once
#include <vector>
#include <string>
#include <stdexcept>

/**
 * @brief 1D Isı Denklemi için FTCS (Forward-Time Central-Space) Çözücüsü
 *
 * Çözülen PDE:
 *   du/dt = alpha * d²u/dx²
 *
 * Kararlılık şartı (Von Neumann):
 *   r = alpha * dt / dx² <= 0.5
 *
 * Sınır Koşulları: Dirichlet (sabit uç sıcaklıkları)
 */
class HeatSolver {
private:
    int    nx;       ///< Izgara noktası sayısı
    double alpha;    ///< Isı yayınım katsayısı [m²/s]
    double dx;       ///< Uzay adımı [m]
    double dt;       ///< Zaman adımı [s]
    double r;        ///< Sonlu fark katsayısı: alpha*dt/dx² (önceden hesaplanır)

    std::vector<double> u;      ///< Mevcut zaman adımındaki sıcaklıklar [°C]
    std::vector<double> u_next; ///< Bir sonraki zaman adımı için tampon

public:
    /**
     * @brief Çözücüyü başlatır ve kararlılık şartını kontrol eder.
     * @throws std::invalid_argument  Herhangi bir parametre <= 0 ise
     * @throws std::runtime_error     Von Neumann kararlılık şartı (r <= 0.5) ihlal edilirse
     */
    HeatSolver(int grid_points, double diffusivity, double space_step, double time_step);

    /**
     * @brief İç noktalara başlangıç sıcaklığı atar (sınır noktaları hariç).
     * @note  setBoundaryConditions() ÖNCE çağrılmalıdır.
     */
    void setInitialCondition(double internal_temp);

    /**
     * @brief Sabit Dirichlet sınır koşullarını uygular.
     * @note  setInitialCondition() ÇAĞRILMADAN ÖNCE kullanılmalıdır.
     */
    void setBoundaryConditions(double left_temp, double right_temp);

    /**
     * @brief Tek bir FTCS zaman adımını ilerletir.
     *        Bellek kopyası yerine std::swap ile O(1) tampon değişimi yapılır.
     */
    void step();

    /**
     * @brief Mevcut sıcaklık dağılımını CSV dosyasına kaydeder.
     * @throws std::runtime_error Dosya açılamazsa
     */
    void saveToCSV(const std::string& filename) const;

    /** @brief Mevcut sıcaklık vektörüne salt-okunur erişim. */
    const std::vector<double>& getTemperature() const { return u; }

    /** @brief Hesaplanan r katsayısını döndürür (test/doğrulama için). */
    double getStabilityCoefficient() const { return r; }
};