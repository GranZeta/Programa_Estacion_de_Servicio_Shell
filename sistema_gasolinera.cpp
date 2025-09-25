#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

struct Producto {
    int codigo;
    char nombre[30];
    double precio;
    bool activo;
};

struct Venta {
    int ticket;
    int codigoProducto;
    int cantidad;
    double total;
    char vendedor[20];
    bool activa;
};

struct Nodo {
    Venta dato;
    Nodo *sig;
};

class SistemaGasolinera {
private:
    map<int, Producto> productos;

    int leerEntero(string mensaje, int min, int max) {
        int n;
        while (true) {
            cout << mensaje;
            if (cin >> n && n >= min && n <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return n;
            }
            cout << "Error. Ingrese un numero entre " << min << " y " << max << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    double leerDouble(string mensaje) {
        double n;
        while (true) {
            cout << mensaje;
            if (cin >> n && n > 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return n;
            }
            cout << "Error. Ingrese un numero positivo.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void leerCadena(string mensaje, char *dest, int tam) {
        cout << mensaje;
        cin.getline(dest, tam);
    }

    vector<Producto> obtenerProductosDesdeArchivo() {
        vector<Producto> lista;
        ifstream f("productos.dat", ios::binary);
        if (!f) return lista;
        Producto p;
        while (f.read((char*)&p, sizeof(Producto))) {
            lista.push_back(p);
        }
        f.close();
        return lista;
    }

    Producto buscarProducto(int codigo) {
        if (productos.count(codigo)) return productos[codigo];
        return Producto{0,"",0,false};
    }

    int generarTicket() {
        ifstream f("ventas.dat", ios::binary);
        if (!f) return 1;
        Venta v; int max=0;
        while (f.read((char*)&v, sizeof(Venta))) 
            if (v.ticket>max) max=v.ticket;
        f.close();
        return max+1;
    }

public:
    SistemaGasolinera() {
        cargarProductosEnMemoria();
    }

    void cargarProductosIniciales() {
        double dolar = leerDouble("Ingrese valor del dolar: ");
        fstream f("productos.dat", ios::out | ios::binary);
        if (!f) { cerr << "Error creando archivo productos.dat\n"; return; }
        Producto p[] = {
            {1, "Shell Formula Regular", 4.2 * dolar, true},
            {2, "Shell Formula Super",   4.6 * dolar, true},
            {3, "Shell V-Power Nafta",   5.6 * dolar, true},
            {4, "Evolux Diesel",         5.0 * dolar, true},
            {5, "Shell V-Power Diesel",  6.2 * dolar, true}
        };
        for (auto &x : p) f.write((char*)&x, sizeof(Producto));
        f.close();
        cargarProductosEnMemoria();
        cout << "Productos iniciales cargados.\n";
    }

    void cargarProductosEnMemoria() {
        productos.clear();
        auto lista = obtenerProductosDesdeArchivo();
        for (auto &p : lista) if (p.activo) productos[p.codigo] = p;
    }

    void listarProductos() {
        cout << "\n--- PRODUCTOS ---\n";
        if (productos.empty()) { cout << "No hay productos.\n"; return; }
        for (auto &par : productos)
            cout << par.second.codigo << " - " << par.second.nombre
                 << " - $" << par.second.precio << endl;
    }

    void modificarPrecio() {
        fstream f("productos.dat", ios::in | ios::out | ios::binary);
        if (!f) { cerr << "Error abriendo productos.dat\n"; return; }
        int cod = leerEntero("Codigo producto: ", 1, 1000);
        double nuevo = leerDouble("Nuevo precio: ");
        Producto p; bool ok=false;
        while (f.read((char*)&p, sizeof(Producto))) {
            if (p.codigo==cod && p.activo) {
                p.precio=nuevo;
                f.seekp(- (int)sizeof(Producto), ios::cur);
                f.write((char*)&p, sizeof(Producto));
                productos[cod]=p;
                cout<<"Precio actualizado.\n"; ok=true; break;
            }
        }
        if (!ok) cout<<"Producto no encontrado.\n";
        f.close();
    }

    void desactivarProducto() {
        fstream f("productos.dat", ios::in | ios::out | ios::binary);
        if (!f) { cerr << "Error abriendo productos.dat\n"; return; }
        int cod = leerEntero("Codigo producto a desactivar: ", 1, 1000);
        Producto p; bool ok=false;
        while (f.read((char*)&p, sizeof(Producto))) {
            if (p.codigo==cod && p.activo) {
                p.activo=false;
                f.seekp(- (int)sizeof(Producto), ios::cur);
                f.write((char*)&p, sizeof(Producto));
                productos.erase(cod);
                cout<<"Producto desactivado.\n"; ok=true; break;
            }
        }
        if (!ok) cout<<"Producto no encontrado.\n";
        f.close();
    }

    void registrarVenta() {
        ofstream f("ventas.dat", ios::app | ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        Venta v;
        v.ticket=generarTicket();
        v.codigoProducto=leerEntero("Codigo producto: ",1,1000);
        Producto p=buscarProducto(v.codigoProducto);
        if (!p.activo) { cout<<"Producto invalido.\n"; return; }
        v.cantidad=leerEntero("Cantidad: ",1,1000);
        leerCadena("Vendedor: ", v.vendedor, 20);
        v.total=v.cantidad*p.precio; v.activa=true;
        f.write((char*)&v,sizeof(Venta));
        f.close();
        cout<<"Venta registrada. Ticket "<<v.ticket<<" Total $"<<v.total<<"\n";
    }

    void listarVentas() {
        ifstream f("ventas.dat", ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        Venta v;
        cout<<"\n--- VENTAS ---\n";
        while (f.read((char*)&v,sizeof(Venta)))
            if (v.activa)
                cout<<"Ticket:"<<v.ticket<<" Prod:"<<v.codigoProducto
                    <<" Cant:"<<v.cantidad<<" Total:$"<<v.total
                    <<" Vendedor:"<<v.vendedor<<"\n";
        f.close();
    }

    void anularVenta() {
        fstream f("ventas.dat", ios::in | ios::out | ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        int t=leerEntero("Ticket a anular: ",1,100000);
        Venta v; bool ok=false;
        while (f.read((char*)&v,sizeof(Venta))) {
            if (v.ticket==t && v.activa) {
                v.activa=false;
                f.seekp(- (int)sizeof(Venta), ios::cur);
                f.write((char*)&v,sizeof(Venta));
                cout<<"Venta anulada.\n"; ok=true; break;
            }
        }
        if (!ok) cout<<"Ticket no encontrado.\n";
        f.close();
    }

    void recaudacionTotal() {
        ifstream f("ventas.dat", ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        double total=0; Venta v;
        while (f.read((char*)&v,sizeof(Venta))) if (v.activa) total+=v.total;
        cout<<"Recaudacion total: $"<<total<<"\n";
        f.close();
    }

    void recaudacionPorProducto() {
        ifstream f("ventas.dat", ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        map<int,double> suma; Venta v;
        while (f.read((char*)&v,sizeof(Venta))) if (v.activa) suma[v.codigoProducto]+=v.total;
        f.close();
        vector<pair<string,double>> rep;
        for (auto &par:suma) if (productos.count(par.first)) 
            rep.push_back({productos[par.first].nombre,par.second});
        sort(rep.begin(),rep.end(),[](auto &a,auto &b){return a.second>b.second;});
        cout<<"\n--- RECAUDACION POR PRODUCTO ---\n";
        for (auto &r:rep) cout<<r.first<<": $"<<r.second<<"\n";
    }

    void ventasEnLista() {
        ifstream f("ventas.dat", ios::binary);
        if (!f) { cerr<<"Error abriendo ventas.dat\n"; return; }
        Nodo* lista=nullptr; Venta v;
        while (f.read((char*)&v,sizeof(Venta)))
            if (v.activa) lista=new Nodo{v,lista};
        f.close();
        cout<<"\n--- LISTA DE VENTAS ---\n";
        while (lista) {
            cout<<"Ticket:"<<lista->dato.ticket<<" Total:$"<<lista->dato.total
                <<" Vendedor:"<<lista->dato.vendedor<<"\n";
            Nodo* aux=lista; lista=lista->sig; delete aux;
        }
    }

    void menu() {
        int op;
        do {
            cout<<"\n====== SISTEMA SHELL ======\n";
            cout<<"1. Cargar productos iniciales\n";
            cout<<"2. Recargar productos en memoria\n";
            cout<<"3. Listar productos\n";
            cout<<"4. Registrar venta\n";
            cout<<"5. Listar ventas\n";
            cout<<"6. Anular venta\n";
            cout<<"7. Modificar precio\n";
            cout<<"8. Desactivar producto\n";
            cout<<"9. Recaudacion total\n";
            cout<<"10. Recaudacion por producto\n";
            cout<<"11. Ventas en lista\n";
            cout<<"0. Salir\nOpcion: ";
            op=leerEntero("",0,11);
            switch(op) {
                case 1:cargarProductosIniciales();break;
                case 2:cargarProductosEnMemoria();break;
                case 3:listarProductos();break;
                case 4:registrarVenta();break;
                case 5:listarVentas();break;
                case 6:anularVenta();break;
                case 7:modificarPrecio();break;
                case 8:desactivarProducto();break;
                case 9:recaudacionTotal();break;
                case 10:recaudacionPorProducto();break;
                case 11:ventasEnLista();break;
            }
        } while(op!=0);
    }
};

int main() {
    SistemaGasolinera sistema;
    sistema.menu();
    return 0;
}