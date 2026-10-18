#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Pokemon {
    int numeroPokedex;
    string nombre;
    string tipo1;
    string tipo2;
    int total;
    int hp;
    int ataque;
    int defensa;
    int ataqueEspecial;
    int defensaEspecial;
    int velocidad;
    int generacion;
    bool legendario;
};

string aMinusculas(const string& texto) {
    string resultado = texto;
    transform(resultado.begin(), resultado.end(), resultado.begin(),
              [](unsigned char caracter) { return static_cast<char>(tolower(caracter)); });
    return resultado;
}

vector<string> separarCSV(const string& linea) {
    vector<string> campos;
    string campo;
    bool entreComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        char caracter = linea[i];
        if (caracter == '"') {
            if (entreComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                campo += '"';
                ++i;
            } else {
                entreComillas = !entreComillas;
            }
        } else if (caracter == ',' && !entreComillas) {
            campos.push_back(campo);
            campo.clear();
        } else {
            campo += caracter;
        }
    }

    campos.push_back(campo);
    return campos;
}

bool convertirEntero(const string& texto, int& valor) {
    istringstream entrada(texto);
    entrada >> valor;
    return !entrada.fail() && entrada.peek() == char_traits<char>::eof();
}

bool cargarPokemons(const string& nombreArchivo, vector<Pokemon>& pokemons) {
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        return false;
    }

    string linea;
    if (!getline(archivo, linea)) {
        cerr << "El archivo CSV esta vacio." << endl;
        return false;
    }

    int numeroLinea = 1;
    while (getline(archivo, linea)) {
        ++numeroLinea;
        if (!linea.empty() && linea[linea.size() - 1] == '\r') {
            linea.erase(linea.size() - 1);
        }
        if (linea.empty()) {
            continue;
        }

        vector<string> campos = separarCSV(linea);
        Pokemon pokemon;
        if (campos.size() != 13 ||
            !convertirEntero(campos[0], pokemon.numeroPokedex) ||
            !convertirEntero(campos[4], pokemon.total) ||
            !convertirEntero(campos[5], pokemon.hp) ||
            !convertirEntero(campos[6], pokemon.ataque) ||
            !convertirEntero(campos[7], pokemon.defensa) ||
            !convertirEntero(campos[8], pokemon.ataqueEspecial) ||
            !convertirEntero(campos[9], pokemon.defensaEspecial) ||
            !convertirEntero(campos[10], pokemon.velocidad) ||
            !convertirEntero(campos[11], pokemon.generacion) ||
            (campos[12] != "True" && campos[12] != "False")) {
            cerr << "Formato invalido en la linea " << numeroLinea
                 << " del archivo CSV." << endl;
            return false;
        }

        pokemon.nombre = campos[1];
        pokemon.tipo1 = campos[2];
        pokemon.tipo2 = campos[3];
        pokemon.legendario = campos[12] == "True";
        pokemons.push_back(pokemon);
    }

    if (pokemons.empty()) {
        cerr << "El archivo CSV no contiene registros de Pokemon." << endl;
        return false;
    }
    return true;
}

bool leerEntero(int& valor) {
    string linea;
    if (!getline(cin, linea)) {
        return false;
    }

    istringstream entrada(linea);
    string sobrante;
    if (!(entrada >> valor) || (entrada >> sobrante)) {
        return false;
    }
    return true;
}

void mostrarInfoPokemon(const Pokemon& pokemon) {
    cout << "\nNumero de Pokedex: " << pokemon.numeroPokedex << endl;
    cout << "Nombre: " << pokemon.nombre << endl;
    cout << "Tipo(s): " << pokemon.tipo1;
    if (!pokemon.tipo2.empty()) {
        cout << " / " << pokemon.tipo2;
    }
    cout << endl;
    cout << "Total: " << pokemon.total << endl;
    cout << "HP: " << pokemon.hp << endl;
    cout << "Ataque: " << pokemon.ataque << endl;
    cout << "Defensa: " << pokemon.defensa << endl;
    cout << "Ataque especial: " << pokemon.ataqueEspecial << endl;
    cout << "Defensa especial: " << pokemon.defensaEspecial << endl;
    cout << "Velocidad: " << pokemon.velocidad << endl;
    cout << "Generacion: " << pokemon.generacion << endl;
    cout << "Legendario: " << (pokemon.legendario ? "Si" : "No") << endl;
}

void buscarPorPokedex(const vector<Pokemon>& pokemons) {
    cout << "Ingrese el numero de Pokedex: ";
    int numero;
    if (!leerEntero(numero)) {
        cout << "Entrada invalida." << endl;
        return;
    }

    for (size_t i = 0; i < pokemons.size(); ++i) {
        if (pokemons[i].numeroPokedex == numero) {
            mostrarInfoPokemon(pokemons[i]);
            return;
        }
    }
    cout << "No se encontro un Pokemon con ese numero de Pokedex." << endl;
}

void buscarPorNombre(const vector<Pokemon>& pokemons) {
    cout << "Ingrese el nombre o parte del nombre: ";
    string consulta;
    if (!getline(cin, consulta) || consulta.empty()) {
        cout << "El nombre no puede estar vacio." << endl;
        return;
    }

    const string consultaMinuscula = aMinusculas(consulta);
    int encontrados = 0;
    cout << "\nResultados de la busqueda:" << endl;
    for (size_t i = 0; i < pokemons.size(); ++i) {
        if (aMinusculas(pokemons[i].nombre).find(consultaMinuscula) != string::npos) {
            cout << "#" << pokemons[i].numeroPokedex << " - " << pokemons[i].nombre
                 << " (" << pokemons[i].tipo1;
            if (!pokemons[i].tipo2.empty()) {
                cout << " / " << pokemons[i].tipo2;
            }
            cout << ")" << endl;
            ++encontrados;
        }
    }

    if (encontrados == 0) {
        cout << "No se encontraron Pokemon con ese nombre." << endl;
    }
}

void mostrarPorTipo(const vector<Pokemon>& pokemons) {
    vector<string> tipos;
    for (size_t i = 0; i < pokemons.size(); ++i) {
        if (find(tipos.begin(), tipos.end(), pokemons[i].tipo1) == tipos.end()) {
            tipos.push_back(pokemons[i].tipo1);
        }
        if (!pokemons[i].tipo2.empty() &&
            find(tipos.begin(), tipos.end(), pokemons[i].tipo2) == tipos.end()) {
            tipos.push_back(pokemons[i].tipo2);
        }
    }
    sort(tipos.begin(), tipos.end());

    cout << "\nTipos disponibles:" << endl;
    for (size_t i = 0; i < tipos.size(); ++i) {
        cout << i + 1 << ". " << tipos[i] << endl;
    }
    cout << "Seleccione un tipo: ";

    int seleccion;
    if (!leerEntero(seleccion) || seleccion < 1 ||
        seleccion > static_cast<int>(tipos.size())) {
        cout << "Seleccion invalida." << endl;
        return;
    }

    const string tipoSeleccionado = tipos[seleccion - 1];
    int encontrados = 0;
    cout << "\nPokemon de tipo " << tipoSeleccionado << ":" << endl;
    for (size_t i = 0; i < pokemons.size(); ++i) {
        if (pokemons[i].tipo1 == tipoSeleccionado ||
            pokemons[i].tipo2 == tipoSeleccionado) {
            cout << "#" << pokemons[i].numeroPokedex << " - " << pokemons[i].nombre
                 << " | Total: " << pokemons[i].total << endl;
            ++encontrados;
        }
    }
    cout << "Cantidad: " << encontrados << endl;
}

void mostrarInfoPorNombre(const vector<Pokemon>& pokemons) {
    cout << "Ingrese el nombre exacto del Pokemon: ";
    string nombre;
    if (!getline(cin, nombre) || nombre.empty()) {
        cout << "El nombre no puede estar vacio." << endl;
        return;
    }

    const string nombreBuscado = aMinusculas(nombre);
    for (size_t i = 0; i < pokemons.size(); ++i) {
        if (aMinusculas(pokemons[i].nombre) == nombreBuscado) {
            mostrarInfoPokemon(pokemons[i]);
            return;
        }
    }
    cout << "No se encontro un Pokemon con ese nombre." << endl;
}

void mostrarRankings(const vector<Pokemon>& pokemons) {
    vector<Pokemon> ordenados = pokemons;
    sort(ordenados.begin(), ordenados.end(),
         [](const Pokemon& a, const Pokemon& b) {
             if (a.total != b.total) {
                 return a.total > b.total;
             }
             return a.numeroPokedex < b.numeroPokedex;
         });

    vector<Pokemon> masDebiles = pokemons;
    sort(masDebiles.begin(), masDebiles.end(),
         [](const Pokemon& a, const Pokemon& b) {
             if (a.total != b.total) {
                 return a.total < b.total;
             }
             return a.numeroPokedex < b.numeroPokedex;
         });

    cout << "\nTop 3 Pokemon mas fuertes (por Total):" << endl;
    for (size_t i = 0; i < 3 && i < ordenados.size(); ++i) {
        cout << i + 1 << ". #" << ordenados[i].numeroPokedex << " - "
             << ordenados[i].nombre << " | Total: " << ordenados[i].total << endl;
    }

    cout << "\nTop 3 Pokemon mas debiles (por Total):" << endl;
    for (size_t i = 0; i < 3 && i < masDebiles.size(); ++i) {
        const Pokemon& pokemon = masDebiles[i];
        cout << i + 1 << ". #" << pokemon.numeroPokedex << " - " << pokemon.nombre
             << " | Total: " << pokemon.total << endl;
    }
}

void mostrarMenu() {
    cout << "\n=== CONSULTA DE POKEMON ===" << endl;
    cout << "1. Ver informacion por numero de Pokedex" << endl;
    cout << "2. Buscar Pokemon por nombre" << endl;
    cout << "3. Ver Pokemon por tipo" << endl;
    cout << "4. Ver informacion por nombre exacto" << endl;
    cout << "5. Ver los 3 mas fuertes y los 3 mas debiles" << endl;
    cout << "6. Salir" << endl;
    cout << "Elija una opcion: ";
}

int main() {
    const string nombreArchivo = "pokemon_150_registros.csv";
    vector<Pokemon> pokemons;
    if (!cargarPokemons(nombreArchivo, pokemons)) {
        return 1;
    }

    cout << "Se cargaron " << pokemons.size() << " Pokemon." << endl;
    int opcion;
    do {
        mostrarMenu();
        if (!leerEntero(opcion)) {
            cout << "\nEntrada invalida. Ingrese un numero del 1 al 6." << endl;
            if (cin.eof()) {
                break;
            }
            continue;
        }

        switch (opcion) {
            case 1:
                buscarPorPokedex(pokemons);
                break;
            case 2:
                buscarPorNombre(pokemons);
                break;
            case 3:
                mostrarPorTipo(pokemons);
                break;
            case 4:
                mostrarInfoPorNombre(pokemons);
                break;
            case 5:
                mostrarRankings(pokemons);
                break;
            case 6:
                cout << "Hasta luego." << endl;
                break;
            default:
                cout << "Opcion invalida. Ingrese un numero del 1 al 6." << endl;
        }
    } while (opcion != 6);

    return 0;
}
