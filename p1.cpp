#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <vector>
#include <unistd.h>

using namespace std;
enum DeletionStrategy {
    MOVE_LAST,
    FREE_LIST
};
struct Alumno{
    char codigo[5];
    char nombre[11];
    char apellidos[20];
    char carrera[15];
    int ciclo;
    float mensualidad;
    Alumno() : ciclo(0), mensualidad(0.0f){
        codigo[0] = '\0';
        nombre[0] = '\0';
        apellidos[0] = '\0';
        carrera[0] = '\0';
    }
    Alumno(const char* _codigo, const char* _nombre, const char* _apellidos,
           const char* _carrera, int _ciclo, float _mensualidad) {
        strncpy(codigo, _codigo, sizeof(codigo) - 1);
        codigo[sizeof(codigo) - 1] = '\0';
        strncpy(nombre, _nombre, sizeof(nombre) - 1);
        nombre[sizeof(nombre) - 1] = '\0';
        strncpy(apellidos, _apellidos, sizeof(apellidos) - 1);
        apellidos[sizeof(apellidos) - 1] = '\0';
        strncpy(carrera, _carrera, sizeof(carrera) - 1);
        carrera[sizeof(carrera) - 1] = '\0';
        ciclo = _ciclo;
        mensualidad = _mensualidad;
    }
};

std::ostream& operator<<(std::ostream& os, const Alumno& alumno) {
    os << "Codigo: " << alumno.codigo << endl
       << "Nombre: " << alumno.nombre << endl
       << "Apellidos: " << alumno.apellidos << endl
       << "Carrera: " << alumno.carrera << endl
       << "Ciclo: " << alumno.ciclo << endl
       << "Mensualidad: " << alumno.mensualidad << endl;
    return os;
}

class FixedRecord {
private:
    string filename;
    DeletionStrategy type_del;
public:
    FixedRecord(string fileName, DeletionStrategy type_del){
        this->filename = fileName;
        this->type_del = type_del;
        inicializar_archivo();
    };

    void inicializar_archivo(){
        ofstream file(this->filename, ios::app);
        file.close();
    }

    vector<Alumno> load(){
        vector<Alumno> test1;
        ifstream myfile ("test.bin", ios::binary);
        Alumno a1;
        while (myfile.read((char*)&a1, sizeof(Alumno))){
            test1.push_back(a1);
        }
        myfile.close();
        return test1;
    }
    void add(const Alumno& record) {
        ofstream file(filename, ios::binary | ios::app);
        file.write(reinterpret_cast<const char*>(&record), sizeof(Alumno));
        file.close();
    }
    Alumno readRecord(int pos) {
        ifstream file(filename, ios::binary);
        Alumno temp;
        file.seekg(pos * sizeof(Alumno));
        file.read(reinterpret_cast<char*>(&temp), sizeof(Alumno));
        file.close();
        return temp;
    }
    bool deleteRecord(int pos) {
        switch (type_del) {
            case MOVE_LAST:
                return deleteMoveLast(pos);
            case FREE_LIST:
                return deleteFreeList(pos);
        }
        return false;
    }
    void borrarTodo(int indice) {
        fstream file("test.bin", ios::in | ios::out | ios::binary);
        file.seekg(0, ios::end);

        int fileSize = file.tellg();
        int totalRecords = fileSize / sizeof(Alumno);

        if (indice < 0 || indice >= totalRecords) {
            cerr << "Índice fuera de rango." << endl;
            file.close();
            return;
        }

        Alumno al;

        for (int i = indice + 1; i < totalRecords; i++) {
            file.seekg(i * sizeof(Alumno));
            file.read(reinterpret_cast<char*>(&al), sizeof(Alumno));
            file.seekp((i - 1) * sizeof(Alumno));
            file.write(reinterpret_cast<const char*>(&al), sizeof(Alumno));
        }

        file.close();

        ofstream truncateFile("datos.bin", ios::binary | ios::trunc | ios::out);
        truncateFile.seekp((totalRecords - 1) * sizeof(Alumno));
        truncateFile.close();
    }
private:
    bool deleteMoveLast(int pos) {
        ifstream fileIn(filename, ios::binary);
        if (!fileIn) {
            fileIn.close();
            return false;
        };

        fileIn.seekg(0, ios::end);
        int size = fileIn.tellg();
        int totalRecords = size / sizeof(Alumno);

        if (pos < 0 || pos >= totalRecords - 1) {
            fileIn.close();
            return false;
        }

        fileIn.seekg((totalRecords - 1) * sizeof(Alumno));
        Alumno lastRecord;
        fileIn.read(reinterpret_cast<char*>(&lastRecord), sizeof(Alumno));
        fileIn.close();

        ofstream fileOut(filename, ios::binary | ios::in | ios::out);
        fileOut.seekp(pos * sizeof(Alumno));
        fileOut.write(reinterpret_cast<const char*>(&lastRecord), sizeof(Alumno));
        fileOut.close();
        truncate(filename.c_str(), (totalRecords - 1) * sizeof(Alumno));
        return true;
    }

    bool deleteFreeList(int pos) {
        ifstream fileIn(filename, ios::binary);
        if (!fileIn) {
            fileIn.close();
            return false;
        };
        fileIn.seekg(0, ios::end);
        int size = fileIn.tellg();
        int totalRecords = size / sizeof(Alumno);



        Alumno lastRecord;
        fileIn.seekg((totalRecords) * sizeof(Alumno));
        fileIn.read(reinterpret_cast<char*>(&lastRecord), sizeof(Alumno));
        fileIn.close();

        ofstream fileOut(filename, ios::binary | ios::in | ios::out);
        fileOut.seekp(pos * sizeof(Alumno));
        fileOut.write(reinterpret_cast<const char*>(&lastRecord), sizeof(Alumno));
        fileOut.close();

        truncate(filename.c_str(), (totalRecords) * sizeof(Alumno));

        fileIn.close();
        return true;
    }

};

int main() {
    FixedRecord File1("test.bin", MOVE_LAST);

    Alumno alumno1("0001", "Juan", "Perez", "Ingenieria", 3, 500.0);
    Alumno alumno2("0002", "Pedro", "Modigez", "Doctor", 5, 250.0);
    File1.inicializar_archivo();
    File1.add(alumno1);
    File1.add(alumno2);

    vector<Alumno> Alumnos = File1.load();
    for(auto alumno: Alumnos){
        cout<< "---------" << endl;
        cout<< "Cargando Alumno:" << endl;
        cout<< alumno;
        cout<< "---------" << endl;
    }
    cout << "Alumno por readRecord"<<endl;
    cout << File1.readRecord(0);

    File1.deleteRecord(0);
    cout<< "Verificando que se borro" << endl;
     vector<Alumno> Alumnos2 = File1.load();
//
    for(auto alumno: Alumnos2){
        cout<<"Alumno---------"<<endl;
        cout<<alumno;
    }
//    File1.borrarTodo(0);
//    vector<Alumno> Alumnos2 = File1.load();
//    for(auto alumno: Alumnos2){
//        cout<< "---------" << endl;
//        cout<< "Alumno" << endl;
//        cout<< alumno;
//        cout<< "---------" << endl;
//    }
    return 0;
}
