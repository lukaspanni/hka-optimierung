#include <iostream>
#include <cmath>

double power(double x, size_t n)
{
    return pow(x, n);
}

double faculty(double n)
{
    return tgamma(n + 1);
}

double e_hoch_x(double x, const size_t ITERATIONEN = 1000)
{
    double e_hoch_x = 1.0;

    for (size_t n = 1; n < ITERATIONEN; n++)
    {
        e_hoch_x += power(x, n) / faculty(n);
    }

    return e_hoch_x;
}

// Ihre optimierte Version bei der auf eine lokale Variable in jedem
// Schleifendurchlauf x/n hinzumulitpliziert wird, um
// x^n / n! iterativ zu berechnen, statt power und faculty aufzurufen
// es werden dadurch wiederholte Berechnungen vermieden
//
// zum Aufruf in der main-Methode vertauschen sie die Namen dieser Funktion
// mit der obigen Variante
double e_hoch_x_1(double x, const size_t ITERATIONEN = 1000)
{
    double e_hoch_x = 1.0;

    double quotient = 1.0;
    for (size_t n = 1; n < ITERATIONEN; n++)
    {
        quotient *= x / n;
        e_hoch_x += quotient;
    }

    return e_hoch_x;
}

int main(void)
{
    // ein Testaufruf
    std::cout << e_hoch_x_1(1.0) << std::endl; // Die Eulersche Zahl e sollte ausgegeben werden

    // ab hier viele Aufrufe durchführen
    double e = 0.0;
    for (size_t i = 0; i < 10000; i++)
    {
        e += (i % 2 == 0 ? -1 : 1) * e_hoch_x_1(1.0);
    }

    std::cout << e << std::endl; // es sollte 0 herauskommen
    return 1;
}
