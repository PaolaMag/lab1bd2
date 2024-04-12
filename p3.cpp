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
    VariableLengthRecord(string filename, string metaFilename) : filename(filename), metaFilename(metaFilename) {}

    void add(const Matricula& record) {
        std::ofstream file(filename, ios::binary | ios::app | ios::in | ios::out);
        std::ofstream metaFile(metaFilename, ios::binary | ios::app | ios::in | ios::out);

        if (!file.is_open() || !metaFile.is_open()) {
            cerr << "No se pudo abrir el archivo" << endl;
            return;
        }

        file.seekp(0, ios::end);
        size_t pos = file.tellp();
        metaFile.write(reinterpret_cast<const char*>(&pos), sizeof(pos));


        writeString(file, record.codigo);
        file.write(reinterpret_cast<const char*>(&record.ciclo), sizeof(record.ciclo));
        file.write(reinterpret_cast<const char*>(&record.mensualidad), sizeof(record.mensualidad));
        writeString(file, record.observaciones);

        file.close();
        metaFile.close();

    }

    vector<Matricula> load() {
        std::ifstream file(filename, ios::binary);
        std::ifstream metaFile(metaFilename, ios::binary);
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

    Matricula readRecord(int rPos) {
        std::ifstream file(filename, ios::binary);
        std::ifstream metaFile(metaFilename, ios::binary);

        if (!file.is_open() || !metaFile.is_open()) {
            throw runtime_error("No se pudo abrir el archivo para leer");
        }

        metaFile.seekg(rPos * sizeof(size_t));
        size_t pos;
        metaFile.read(reinterpret_cast<char*>(&pos), sizeof(pos));

        file.seekg(pos);

        Matricula matricula = readMatricula(file);

        file.close();
        metaFile.close();
        return matricula;
    }


private:
    static void writeString(ofstream& out, const string& str) {
        int size = str.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
        out.write(str.c_str(), size);
    }
    static string readString(ifstream& in) {
        int length;
        in.read(reinterpret_cast<char*>(&length), sizeof(int));

        std::vector<char> buffer(length);
        in.read(buffer.data(), length);
        return string(buffer.begin(), buffer.end());
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
    db.add({"0001", 1, 1500.0f, "Primera matricula"});
    db.add({"0002", 2, 1600.0f, "Segunda matricula"});
    db.add({"0003", 5, 2600.0f, "Segunda matricula"});
    db.add({"0004", 7, 3600.0f, "Segunda matricula"});
    db.add({"0005", 9, 6600.0f, "Segunda matricula"});

    // LOAD
    auto matriculas = db.load();
    for (const auto& m : matriculas) {
        cout << "Codigo: " << m.codigo << ", Ciclo: " << m.ciclo << ", Mensualidad: " << m.mensualidad << ", Observaciones: " << m.observaciones << endl;
    }

    // READ
    try {
        auto matricula = db.readRecord(1); // lee el segundo registro
        //output: Codigo: 0002, Ciclo: 2, Mensualidad: 1600, Observaciones: Segunda matricula
        cout << "Registro resultante -> Codigo: " << matricula.codigo << ", Ciclo: " << matricula.ciclo << ", Mensualidad: " << matricula.mensualidad << ", Observaciones: " << matricula.observaciones << endl;
    } catch (const std::exception& e) {
        cerr << "Error al leer el registro: " << e.what() << endl;
    }

    return 0;
}
