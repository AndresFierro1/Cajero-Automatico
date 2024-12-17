#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

class Cajero {
private:
    string nombre, nip;
    double saldo;
    vector<string> movs;
    static double saldoCajero;

public:
    Cajero(string _nombre, string _nip, double _saldo = 0) : nombre(_nombre), nip(_nip), saldo(_saldo) {}

    bool iniciarSesion(const string& _nombre, const string& _nip) {
        return nombre == _nombre && nip == _nip;
    }

    void consSaldo() {
        cout << "Saldo: $" << saldo << endl;
        movs.push_back("Consulta saldo: $" + to_string(saldo));
        guardarMovimiento("Consulta saldo", saldo);  // Guardamos el movimiento
    }

    string calcularDesglose(double cantidad) {
        int billetes500 = 0, billetes200 = 0, billetes100 = 0, billetes50 = 0, billetes20 = 0;

        billetes500 = cantidad / 500;
        cantidad -= billetes500 * 500;

        billetes200 = cantidad / 200;
        cantidad -= billetes200 * 200;

        billetes100 = cantidad / 100;
        cantidad -= billetes100 * 100;

        billetes50 = cantidad / 50;
        cantidad -= billetes50 * 50;

        billetes20 = cantidad / 20;
        cantidad -= billetes20 * 20;

        if (cantidad > 0) {
            cout << "No es posible retirar la cantidad exacta con los billetes disponibles." << endl;
            return "";
        }

        // Generar el desglose
        stringstream ss;
        ss << "Billetes - ";
        if (billetes500 > 0) ss << "500 - " << billetes500 << ", ";
        if (billetes200 > 0) ss << "200 - " << billetes200 << ", ";
        if (billetes100 > 0) ss << "100 - " << billetes100 << ", ";
        if (billetes50 > 0) ss << "50 - " << billetes50 << ", ";
        if (billetes20 > 0) ss << "20 - " << billetes20;

        string desglose = ss.str();
        if (desglose.back() == ' ') {
            desglose = desglose.substr(0, desglose.size() - 2);
        }

        return desglose;
    }

    void retirar(double cantidad) {
        if (cantidad <= saldo) {
            string desglose = calcularDesglose(cantidad);
            if (!desglose.empty()) {
                saldo -= cantidad;
                cout << "Retirado: $" << cantidad << endl;
                cout << desglose << endl;
                movs.push_back("Retiro: $" + to_string(cantidad) + " - " + desglose);
                guardarMovimiento("Retiro", cantidad, desglose);
            }
        }
        else {
            cout << "Saldo insuficiente." << endl;
        }
    }

    void depositar(double cantidad) {
        if (!esCombinacionValida(cantidad)) {
            cout << "La cantidad no es válida." << endl;
            return;
        }

        saldo += cantidad;
        saldoCajero += cantidad;  // Incrementamos el saldo del cajero también
        cout << "Depositado: $" << cantidad << endl;
        movs.push_back("Depósito: $" + to_string(cantidad));
        guardarMovimiento("Depósito", cantidad);  // Guardamos el movimiento
    }

    // Función para validar si la cantidad puede ser formada con los billetes disponibles
    bool esCombinacionValida(double cantidad) {
        int billetes500 = 0, billetes200 = 0, billetes100 = 0, billetes50 = 0, billetes20 = 0;

        // Intentamos formar la cantidad con los billetes de mayor a menor
        billetes500 = cantidad / 500;
        cantidad -= billetes500 * 500;

        billetes200 = cantidad / 200;
        cantidad -= billetes200 * 200;

        billetes100 = cantidad / 100;
        cantidad -= billetes100 * 100;

        billetes50 = cantidad / 50;
        cantidad -= billetes50 * 50;

        billetes20 = cantidad / 20;
        cantidad -= billetes20 * 20;

        // Si después de restar la cantidad se vuelve cero, entonces la combinación es válida
        return cantidad == 0;
    }

    void transferir(double cantidad, string destino) {
        if (cantidad <= saldo) {
            saldo -= cantidad;
            cout << "Transferido: $" << cantidad << " a: " << destino << endl;
            movs.push_back("Transferencia a " + destino + ": $" + to_string(cantidad));
            guardarMovimiento("Transferencia a " + destino, cantidad);  // Guardamos el movimiento
        }
        else {
            cout << "Saldo insuficiente." << endl;
        }
    }

    void pagar(string servicio, double cantidad) {
        if (cantidad <= saldo) {
            saldo -= cantidad;
            cout << "Pagado " << servicio << ": $" << cantidad << endl;
            movs.push_back("Pago de " + servicio + ": $" + to_string(cantidad));
            guardarMovimiento("Pago de " + servicio, cantidad);  // Guardamos el movimiento
        }
        else {
            cout << "Saldo insuficiente." << endl;
        }
    }

    void cambiarNip() {
        string nipActual, nipNuevo, nipConfirmado;

        cout << "Ingresa tu NIP actual: ";
        cin >> nipActual;

        if (nipActual == nip) {
            cout << "Ingresa tu nuevo NIP: ";
            cin >> nipNuevo;
            cout << "Confirma tu nuevo NIP: ";
            cin >> nipConfirmado;

            if (nipNuevo == nipConfirmado) {
                nip = nipNuevo;  // Cambio de NIP
                cout << "NIP cambiado con éxito." << endl;
                movs.push_back("Cambio de NIP.");
                guardarMovimiento("Cambio de NIP", 0);  // Guardamos el cambio de NIP en el archivo
            }
            else {
                cout << "Los NIPs no coinciden. El cambio no se ha realizado." << endl;
            }
        }
        else {
            cout << "El NIP actual es incorrecto. El cambio no se ha realizado." << endl;
        }
    }

    void consMovs() {
        cout << "Movimientos de " << nombre << ":" << endl;
        if (movs.empty()) {
            cout << "No hay movimientos." << endl;
        }
        else {
            for (const auto& mov : movs) {
                cout << "- " << mov << endl;
            }
        }
    }

    static vector<Cajero> cargarCuentas(const string& archivo) {
        vector<Cajero> cuentas;
        ifstream file(archivo);
        string line, nombre, nip;
        double saldo;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, nombre, ',');
            getline(ss, nip, ',');
            ss >> saldo;
            cuentas.push_back(Cajero(nombre, nip, saldo));
        }
        return cuentas;
    }

    static void guardarCuentas(const string& archivo, const vector<Cajero>& cuentas) {
        ofstream file(archivo, ios::trunc);
        for (const auto& cuenta : cuentas) {
            file << cuenta.nombre << "," << cuenta.nip << "," << cuenta.saldo << endl;
        }
    }

    static void cargarSaldoCajero(const string& archivo) {
        ifstream file(archivo);
        if (file.is_open()) {
            file >> saldoCajero;  // Leer el saldo inicial del cajero
            file.close();
        }
        else {
            // Si no existe el archivo, usamos un valor por defecto
            saldoCajero = 10000;
        }
    }

    static void guardarSaldoCajero(const string& archivo) {
        ofstream file(archivo, ios::trunc);
        file << saldoCajero;  // Guardar el saldo final del cajero
        file.close();
    }

    static void agregarDineroAlCajero(double cantidad) {
        saldoCajero += cantidad;
        cout << "Dinero agregado al cajero: $" << cantidad << endl;
    }

    static double obtenerSaldoCajero() {
        return saldoCajero;
    }

    static void adminVerSaldo() {
        cout << "Saldo actual en el cajero: $" << saldoCajero << endl;
    }

    // Cargar los movimientos desde el archivo
    void cargarMovimientos() {
        ifstream file("movimientos.txt");
        string line;
        while (getline(file, line)) {
            if (line.find(nombre) != string::npos) {
                movs.push_back(line);
            }
        }
        file.close();
    }

    // Guardar un movimiento en el archivo
    void guardarMovimiento(const string& accion, double cantidad) {
        ofstream file("movimientos.txt", ios::app);
        file << nombre << ": " << accion << " - $" << cantidad << endl;
        file.close();
    }

    // Guardar el movimiento con desglose de billetes
    void guardarMovimiento(const string& accion, double cantidad, const string& desglose) {
        ofstream file("movimientos.txt", ios::app);
        file << nombre << ": " << accion << " - $" << cantidad << " - " << desglose << endl;  // Nuevo formato
        file.close();
    }
};

double Cajero::saldoCajero = 0;

// Función para validar la entrada de número entero
bool esNumeroValido(int& num) {
    if (cin >> num) {
        return true;
    }
    else {
        cin.clear(); // Limpiar el estado de error de cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar caracteres no válidos
        return false;
    }
}

int main() {
    vector<Cajero> cuentas = Cajero::cargarCuentas("cuentas.txt");

    // Cargar el saldo del cajero desde el archivo
    Cajero::cargarSaldoCajero("saldo.txt");

    string nombre, nip, destino, servicio;
    double cantidad;
    int opcion;
    bool sesionIniciada = false;
    Cajero* cajeroActual = nullptr;

    cout << "------------------------------------------------------------" << endl;
    cout << "Bienvenido al cajero automático." << endl;

    do {
        cout << "\n1. Iniciar sesión\n2. Salir\nElige opción: ";
        while (!esNumeroValido(opcion)) {
            cout << "Entrada inválida. Por favor ingresa un número válido: ";
        }

        // Limpiar el buffer para evitar problemas de entrada después de getline
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opcion) {
        case 1:
            cout << "Usuario: ";
            getline(cin, nombre);
            cout << "NIP: ";
            getline(cin, nip);  // Cambio aquí para leer el NIP en una nueva línea

            if (nombre == "Admin" && nip == "Admin") {  // Si es Admin puede agregar dinero
                cout << "Accediendo como Admin..." << endl;
                cout << "Saldo actual en el cajero: $" << Cajero::obtenerSaldoCajero() << endl;
                cout << "Cantidad a agregar al cajero: $";
                cin >> cantidad;
                Cajero::agregarDineroAlCajero(cantidad);
                Cajero::guardarCuentas("cuentas.txt", cuentas);  // Guardar cambios en el archivo
                break;
            }

            for (auto& cuenta : cuentas) {
                if (cuenta.iniciarSesion(nombre, nip)) {
                    cajeroActual = &cuenta;
                    sesionIniciada = true;
                    cajeroActual->cargarMovimientos();  // Cargamos los movimientos del usuario
                    break;
                }
            }

            if (sesionIniciada) {
                do {
                    cout << "\n1. Consultar saldo\n2. Retirar dinero\n3. Depositar dinero\n4. Transferir dinero\n";
                    cout << "5. Pagar servicios\n6. Cambiar NIP\n7. Consultar movimientos\n8. Salir\nOpción: ";

                    while (!esNumeroValido(opcion)) {
                        cout << "Entrada inválida. Por favor ingresa un número válido: ";
                    }

                    // Limpiar el buffer antes de cada opción del menú
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    switch (opcion) {
                    case 1: cajeroActual->consSaldo(); break;
                    case 2:
                        cout << "Cantidad a retirar: ";
                        cin >> cantidad;
                        cajeroActual->retirar(cantidad);
                        break;
                    case 3:
                        cout << "Cantidad a depositar: ";
                        cin >> cantidad;
                        cajeroActual->depositar(cantidad);
                        break;
                    case 4:
                        cout << "Cantidad a transferir: ";
                        cin >> cantidad;
                        cout << "Cuenta destino: ";
                        cin >> destino;
                        cajeroActual->transferir(cantidad, destino);
                        break;
                    case 5:
                        cout << "Seleccione el servicio a pagar:\n";
                        cout << "1. Agua\n2. Luz\n3. Internet\n4. Recarga de saldo\n";
                        cout << "Elige opción: ";
                        int servicioOpcion;
                        while (!esNumeroValido(servicioOpcion)) {
                            cout << "Entrada inválida. Por favor ingresa un número válido: ";
                        }

                        switch (servicioOpcion) {
                        case 1:
                            servicio = "Agua";
                            break;
                        case 2:
                            servicio = "Luz";
                            break;
                        case 3:
                            servicio = "Internet";
                            break;
                        case 4:
                            servicio = "Recarga de saldo";
                            break;
                        default:
                            cout << "Opción no válida." << endl;
                            continue;
                        }

                        cout << "Cantidad a pagar para " << servicio << ": $";
                        cin >> cantidad;
                        cajeroActual->pagar(servicio, cantidad);
                        break;
                    case 6:
                        cajeroActual->cambiarNip();
                        Cajero::guardarCuentas("cuentas.txt", cuentas);  // Guardar cambios en las cuentas
                        break;
                    case 7: cajeroActual->consMovs(); break;
                    case 8:
                        cout << "Gracias, ¡adiós!" << endl;
                        Cajero::guardarCuentas("cuentas.txt", cuentas);  // Guardar cambios en las cuentas
                        break;
                    default: cout << "Opción no válida." << endl;
                    }
                } while (opcion != 8);
            }
            else {
                cout << "Datos erróneos. El nombre o el NIP no coinciden." << endl;
            }
            break;

        case 2:
            cout << "Saliendo del cajero. ¡Gracias por usar nuestros servicios!" << endl;
            Cajero::guardarSaldoCajero("saldo.txt");
            Cajero::guardarCuentas("cuentas.txt", cuentas);  // Guardar cambios en las cuentas
            break;

        default:
            cout << "Opción no válida." << endl;
        }

    } while (opcion != 2);
    return 0;
}