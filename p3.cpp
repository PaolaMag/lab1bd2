#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Matricula {
    string codigo;
    int ciclo;
    float mensualidad;
    string observaciones;
};

class VariableLengthRecord {
private:
    string filename;
    string metaFilename;

public:
    VariableLengthRecord(const string& filename, const string& metaFilename) : filename(filename), metaFilename(metaFilename) {}

    void add(const Matricula& record) {
        ofstream file(filename, ios::binary | ios::app);
        ofstream metaFile(metaFilename, ios::binary | ios::app);
        if (!file.is_open() || !metaFile.is_open()) {
            cerr << "No se pudo abrir el archivo" << endl;
            return;
        }

        file.seekp(0, ios::end);
        size_t pos = file.tellp();

        metaFile.write(reinterpret_cast<const char*>(&pos), sizeof(pos));

        size_t size = record.codigo.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(record.codigo.c_str(), size);

        file.write(reinterpret_cast<const char*>(&record.ciclo), sizeof(record.ciclo));
        file.write(reinterpret_cast<const char*>(&record.mensualidad), sizeof(record.mensualidad));

        size = record.observaciones.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(record.observaciones.c_str(), size);
    }

    vector<Matricula> load() {
        ifstream file(filename, ios::binary);
        ifstream metaFile(metaFilename, ios::binary);
        vector<Matricula> records;

        if (!file.is_open() || !metaFile.is_open()) {
            throw runtime_error("No se pudo abrir el archivo para leer");
        }

        size_t pos;
        while (metaFile.read(reinterpret_cast<char*>(&pos), sizeof(pos))) {
            file.seekg(pos);
            Matricula m = readMatricula(file);
            records.push_back(m);
        }

        return records;
    }

    Matricula readRecord(int recordPos) {
        ifstream file(filename, ios::binary);
        ifstream metaFile(metaFilename, ios::binary);
        if (!file.is_open() || !metaFile.is_open()) {
            throw runtime_error("No se pudo abrir el archivo para leer");
        }

        metaFile.seekg(recordPos * sizeof(size_t));
        size_t pos;
        metaFile.read(reinterpret_cast<char*>(&pos), sizeof(pos));

        file.seekg(pos);
        return readMatricula(file);
    }

private:
    static void writeString(ofstream& out, const string& str) {
        size_t len = str.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(str.data(), len);
    }

    static string readString(ifstream& in) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        string str(len, ' ');
        in.read(&str[0], len);
        return str;
    }

    static Matricula readMatricula(ifstream& in) {
        Matricula m;
        m.codigo = readString(in);
        in.read(reinterpret_cast<char*>(&m.ciclo), sizeof(m.ciclo));
        in.read(reinterpret_cast<char*>(&m.mensualidad), sizeof(m.mensualidad));
        m.observaciones = readString(in);
        return m;
    }
};

int main() {
    VariableLengthRecord db("registros.bin", "metadata.bin");

    // ADD
    db.add({"A001", 1, 1500.0f, "Primera matricula"});
    db.add({"B002", 2, 1600.0f, "Segunda matricula"});

    // LOAD
    auto matriculas = db.load();
    for (const auto& m : matriculas) {
        cout << "Codigo: " << m.codigo << ", Ciclo: " << m.ciclo << ", Mensualidad: " << m.mensualidad << ", Observaciones: " << m.observaciones << endl;
    }

    // READ
    try {
        auto matricula = db.readRecord(1); // Intenta leer el segundo registro
        cout << "Registro especifico -> Codigo: " << matricula.codigo << ", Ciclo: " << matricula.ciclo << ", Mensualidad: " << matricula.mensualidad << ", Observaciones: " << matricula.observaciones << endl;
    } catch (const std::exception& e) {
        cerr << "Error al leer el registro: " << e.what() << endl;
    }

    return 0;
}
