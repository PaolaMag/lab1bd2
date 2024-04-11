#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Alumno {
    string Nombre;
    string Apellidos;
    string Carrera;
    float mensualidad;
};

class VariableRecord {
private:
    string filename;

public:
    VariableRecord(const string& filename) : filename(filename) {};

    vector<Alumno> load(){
        vector<Alumno> alumnos;
        ifstream file(filename);
        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);
            string item;
            vector<string> items;
            while (getline(ss, item, '|')) {
                items.push_back(item);
            }

            if (items.size() == 4) {
                Alumno alumno;
                alumno.Nombre = items[0];
                alumno.Apellidos = items[1];
                alumno.Carrera = items[2];
                alumno.mensualidad = stof(items[3]); // Convertir string a float
                alumnos.push_back(alumno);
            }
        }
        file.close();
        return alumnos;
    }

    void add(const Alumno& record) {
        ofstream myfile(filename, ios::app);
        myfile << record.Nombre << "|" << record.Apellidos << "|" << record.Carrera << "|" << record.mensualidad << "\n";
        myfile.close();
    }
    Alumno readRecord(int pos) {
        ifstream file(filename);
        string line;
        getline(file, line);

        int currentPos = 0;
        while (getline(file, line)) {
            if (currentPos == pos) {
                stringstream ss(line);
                string item;
                vector<string> items;

                while (getline(ss, item, '|')) {
                    items.push_back(item);
                }

                if (items.size() == 4) {
                    file.close();
                    return {items[0], items[1], items[2], stof(items[3])}; // Asumiendo que siempre hay 4 campos
                }
            }
            currentPos++;
        }
        file.close();
        return Alumno();
    }

};

int main() {
    VariableRecord vr("alumnos.bin");
    vr.add(Alumno{"Howard", "Paredes Zegarra", "Computacion", 1500.00});
    vr.add(Alumno{"Penny", "Vargas Cordero", "Industrial", 2550.50});
    vr.add(Alumno{"Sheldon", "Cooper Quispe", "Mecatronica", 1850.00});
    vector<Alumno> alumnos = vr.load();
    for (const auto& alumno : alumnos) {
        cout << "Nombre: " << alumno.Nombre << ", Apellidos: " << alumno.Apellidos
             << ", Carrera: " << alumno.Carrera << ", Mensualidad: " << alumno.mensualidad << endl;
    }
    Alumno a = vr.readRecord(2);
    cout << "\nRegistro específico:" << endl;
    cout << "Nombre: " << a.Nombre << ", Apellidos: " << a.Apellidos
         << ", Carrera: " << a.Carrera << ", Mensualidad: " << a.mensualidad << endl;

    return 0;
}
