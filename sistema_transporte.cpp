#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <limits>

using namespace std;

/*
  SISTEMA DE GESTION DE TRANSPORTE ENTRE CIUDADES
  Estructuras implementadas manualmente:
  - Lista enlazada de ciudades
  - Grafo no dirigido con listas de adyacencia
  - Lista enlazada de pasajeros
  - Cola enlazada de solicitudes (FIFO)
  - Pila enlazada de historial (LIFO)
  - Arbol binario de busqueda por ID de ciudad
  - BFS, DFS y Dijkstra (seleccion O(V^2))
*/

struct Conexion {
    int destino;
    double distancia;
    double costo;
    Conexion *sig;
};

struct Ciudad {
    int id;
    string nombre;
    string departamento;
    int poblacion;
    Conexion *adyacentes;
    Ciudad *sig;
};

struct Pasajero {
    int id;
    string nombre;
    int edad;
    int ciudadId;
    Pasajero *sig;
};

struct Solicitud {
    int pasajeroId;
    int origenId;
    int destinoId;
    Solicitud *sig;
};

struct Accion {
    string descripcion;
    Accion *sig;
};

struct NodoArbol {
    int id;
    NodoArbol *izq;
    NodoArbol *der;
};

Ciudad *primeraCiudad = NULL;
Pasajero *primerPasajero = NULL;
Solicitud *frente = NULL;
Solicitud *finalCola = NULL;
Accion *topeHistorial = NULL;
NodoArbol *raiz = NULL;

// ---------- Entrada segura ----------

string numeroATexto(int numero) {
    stringstream ss;
    ss << numero;
    return ss.str();
}

string leerTexto(string mensaje) {
    string dato;
    cout << mensaje;
    getline(cin, dato);
    return dato;
}

int leerEntero(string mensaje) {
    string linea;
    int valor;
    char extra;
    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream ss(linea);
        if (ss >> valor && !(ss >> extra)) return valor;
        cout << "Entrada invalida. Escriba un numero entero.\n";
    }
}

double leerReal(string mensaje) {
    string linea;
    double valor;
    char extra;
    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream ss(linea);
        if (ss >> valor && !(ss >> extra) && valor >= 0) return valor;
        cout << "Entrada invalida. Escriba un numero positivo o cero.\n";
    }
}

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    string x;
    getline(cin, x);
}

void registrarAccion(string descripcion) {
    Accion *nuevo = new Accion;
    nuevo->descripcion = descripcion;
    nuevo->sig = topeHistorial;
    topeHistorial = nuevo;
}

Ciudad *buscarCiudad(int id) {
    Ciudad *actual = primeraCiudad;
    while (actual != NULL) {
        if (actual->id == id) return actual;
        actual = actual->sig;
    }
    return NULL;
}

Pasajero *buscarPasajero(int id) {
    Pasajero *actual = primerPasajero;
    while (actual != NULL) {
        if (actual->id == id) return actual;
        actual = actual->sig;
    }
    return NULL;
}

int contarCiudades() {
    int total = 0;
    Ciudad *p = primeraCiudad;
    while (p != NULL) { total++; p = p->sig; }
    return total;
}

int contarConexiones() {
    int total = 0;
    Ciudad *p = primeraCiudad;
    while (p != NULL) {
        Conexion *c = p->adyacentes;
        while (c != NULL) { total++; c = c->sig; }
        p = p->sig;
    }
    return total / 2; // El grafo guarda cada arista en ambos sentidos.
}

int contarPasajeros() {
    int total = 0;
    Pasajero *p = primerPasajero;
    while (p != NULL) { total++; p = p->sig; }
    return total;
}

// ---------- Arbol binario de busqueda ----------

void insertarArbol(int id) {
    NodoArbol *nuevo = new NodoArbol;
    nuevo->id = id;
    nuevo->izq = NULL;
    nuevo->der = NULL;

    if (raiz == NULL) { raiz = nuevo; return; }

    NodoArbol *actual = raiz;
    NodoArbol *padre = NULL;
    while (actual != NULL) {
        padre = actual;
        if (id < actual->id) actual = actual->izq;
        else if (id > actual->id) actual = actual->der;
        else { delete nuevo; return; }
    }
    if (id < padre->id) padre->izq = nuevo;
    else padre->der = nuevo;
}

bool buscarEnArbol(int id) {
    NodoArbol *actual = raiz;
    while (actual != NULL) {
        if (id == actual->id) return true;
        if (id < actual->id) actual = actual->izq;
        else actual = actual->der;
    }
    return false;
}

void liberarArbol(NodoArbol *nodo) {
    if (nodo == NULL) return;
    liberarArbol(nodo->izq);
    liberarArbol(nodo->der);
    delete nodo;
}

void reconstruirArbol() {
    liberarArbol(raiz);
    raiz = NULL;
    Ciudad *p = primeraCiudad;
    while (p != NULL) {
        insertarArbol(p->id);
        p = p->sig;
    }
}

// ---------- Ciudades: lista enlazada ----------

void agregarCiudad() {
    int id = leerEntero("ID de ciudad: ");
    if (id <= 0 || buscarCiudad(id) != NULL) {
        cout << "Error: el ID debe ser positivo y no estar registrado.\n";
        return;
    }

    string nombre = leerTexto("Nombre: ");
    string departamento = leerTexto("Departamento: ");
    int poblacion = leerEntero("Poblacion: ");
    if (nombre.empty() || departamento.empty() || poblacion < 0) {
        cout << "Error: datos incompletos o poblacion negativa.\n";
        return;
    }

    Ciudad *nuevo = new Ciudad;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->departamento = departamento;
    nuevo->poblacion = poblacion;
    nuevo->adyacentes = NULL;
    nuevo->sig = primeraCiudad;
    primeraCiudad = nuevo;

    insertarArbol(id);
    registrarAccion("Se registro la ciudad " + nombre);
    cout << "Ciudad registrada correctamente.\n";
}

void listarCiudades() {
    if (primeraCiudad == NULL) {
        cout << "No hay ciudades registradas.\n";
        return;
    }
    Ciudad *p = primeraCiudad;
    cout << "\n--- CIUDADES ---\n";
    while (p != NULL) {
        cout << "ID: " << p->id << " | " << p->nombre
             << " | " << p->departamento
             << " | Poblacion: " << p->poblacion << "\n";
        p = p->sig;
    }
}

void consultarCiudad() {
    int id = leerEntero("ID a buscar: ");
    if (!buscarEnArbol(id)) {
        cout << "La ciudad no existe en el arbol de busqueda.\n";
        return;
    }
    Ciudad *p = buscarCiudad(id);
    if (p != NULL)
        cout << "Encontrada: " << p->nombre << ", " << p->departamento
             << ", poblacion " << p->poblacion << ".\n";
}

void modificarCiudad() {
    int id = leerEntero("ID de la ciudad a modificar: ");
    Ciudad *p = buscarCiudad(id);
    if (p == NULL) { cout << "Ciudad no encontrada.\n"; return; }

    string nombre = leerTexto("Nuevo nombre: ");
    string departamento = leerTexto("Nuevo departamento: ");
    int poblacion = leerEntero("Nueva poblacion: ");
    if (nombre.empty() || departamento.empty() || poblacion < 0) {
        cout << "Datos invalidos. No se modifico la ciudad.\n";
        return;
    }
    p->nombre = nombre;
    p->departamento = departamento;
    p->poblacion = poblacion;
    registrarAccion("Se modifico la ciudad ID " + numeroATexto(id));
    cout << "Ciudad modificada.\n";
}

void quitarConexionesCiudad(int id) {
    Ciudad *p = primeraCiudad;
    while (p != NULL) {
        Conexion *actual = p->adyacentes;
        Conexion *anterior = NULL;
        while (actual != NULL) {
            if (actual->destino == id) {
                Conexion *borrar = actual;
                if (anterior == NULL) p->adyacentes = actual->sig;
                else anterior->sig = actual->sig;
                actual = actual->sig;
                delete borrar;
            } else {
                anterior = actual;
                actual = actual->sig;
            }
        }
        p = p->sig;
    }
}

void eliminarCiudad() {
    int id = leerEntero("ID de ciudad a eliminar: ");
    Ciudad *actual = primeraCiudad;
    Ciudad *anterior = NULL;
    while (actual != NULL && actual->id != id) {
        anterior = actual;
        actual = actual->sig;
    }
    if (actual == NULL) { cout << "Ciudad no encontrada.\n"; return; }

    // Primero se eliminan las aristas entrantes y salientes.
    quitarConexionesCiudad(id);
    Conexion *c = actual->adyacentes;
    while (c != NULL) {
        Conexion *borrar = c;
        c = c->sig;
        delete borrar;
    }

    if (anterior == NULL) primeraCiudad = actual->sig;
    else anterior->sig = actual->sig;

    string nombre = actual->nombre;
    delete actual;
    reconstruirArbol();

    // Las solicitudes que dependan de la ciudad se conservan fuera:
    // se filtran para evitar referencias a ciudades inexistentes.
    Solicitud *s = frente;
    Solicitud *ant = NULL;
    while (s != NULL) {
        if (s->origenId == id || s->destinoId == id) {
            Solicitud *b = s;
            if (ant == NULL) frente = s->sig;
            else ant->sig = s->sig;
            s = s->sig;
            if (b == finalCola) finalCola = ant;
            delete b;
        } else { ant = s; s = s->sig; }
    }

    // Si el pasajero tenia esa ciudad como ciudad asociada, se deja en 0 (sin ciudad).
    Pasajero *pas = primerPasajero;
    while (pas != NULL) {
        if (pas->ciudadId == id) pas->ciudadId = 0;
        pas = pas->sig;
    }

    registrarAccion("Se elimino la ciudad " + nombre);
    cout << "Ciudad y sus conexiones eliminadas. Referencias relacionadas revisadas.\n";
}

void menuCiudades() {
    int op;
    do {
        cout << "\n=== GESTION DE CIUDADES ===\n"
             << "1. Registrar\n2. Listar\n3. Buscar por ID (arbol)\n"
             << "4. Modificar\n5. Eliminar\n0. Volver\n";
        op = leerEntero("Opcion: ");
        switch (op) {
            case 1: agregarCiudad(); break;
            case 2: listarCiudades(); break;
            case 3: consultarCiudad(); break;
            case 4: modificarCiudad(); break;
            case 5: eliminarCiudad(); break;
            case 0: break;
            default: cout << "Opcion invalida.\n";
        }
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Grafo: lista de adyacencia ----------

bool existeConexion(int origen, int destino) {
    Ciudad *p = buscarCiudad(origen);
    if (p == NULL) return false;
    Conexion *c = p->adyacentes;
    while (c != NULL) {
        if (c->destino == destino) return true;
        c = c->sig;
    }
    return false;
}

void insertarArista(Ciudad *origen, int destino, double distancia, double costo) {
    Conexion *n = new Conexion;
    n->destino = destino;
    n->distancia = distancia;
    n->costo = costo;
    n->sig = origen->adyacentes;
    origen->adyacentes = n;
}

void agregarConexion() {
    int origen = leerEntero("ID ciudad origen: ");
    int destino = leerEntero("ID ciudad destino: ");
    if (origen == destino) { cout << "No se permite conectar una ciudad consigo misma.\n"; return; }

    Ciudad *a = buscarCiudad(origen);
    Ciudad *b = buscarCiudad(destino);
    if (a == NULL || b == NULL) {
        cout << "Error: ambas ciudades deben estar registradas.\n";
        return;
    }
    if (existeConexion(origen, destino)) {
        cout << "Esa conexion ya existe.\n";
        return;
    }

    double distancia = leerReal("Distancia (km): ");
    double costo = leerReal("Costo: ");
    insertarArista(a, destino, distancia, costo);
    insertarArista(b, origen, distancia, costo);
    registrarAccion("Se conectaron " + a->nombre + " y " + b->nombre);
    cout << "Conexion bidireccional creada.\n";
}

void eliminarConexion() {
    int origen = leerEntero("ID origen: ");
    int destino = leerEntero("ID destino: ");
    if (!existeConexion(origen, destino)) {
        cout << "La conexion no existe.\n";
        return;
    }

    int ids[2];
    ids[0] = origen; ids[1] = destino;
    for (int k = 0; k < 2; k++) {
        Ciudad *p = buscarCiudad(ids[k]);
        int objetivo = ids[1-k];
        Conexion *c = p->adyacentes, *ant = NULL;
        while (c != NULL && c->destino != objetivo) { ant = c; c = c->sig; }
        if (c != NULL) {
            if (ant == NULL) p->adyacentes = c->sig;
            else ant->sig = c->sig;
            delete c;
        }
    }
    registrarAccion("Se elimino una conexion");
    cout << "Conexion eliminada en ambos sentidos.\n";
}

void listarConexiones() {
    if (primeraCiudad == NULL) { cout << "No hay ciudades.\n"; return; }
    Ciudad *p = primeraCiudad;
    bool hay = false;
    cout << "\n--- CONEXIONES ---\n";
    while (p != NULL) {
        Conexion *c = p->adyacentes;
        while (c != NULL) {
            if (p->id < c->destino) {
                Ciudad *dest = buscarCiudad(c->destino);
                cout << p->nombre << " <-> " << (dest ? dest->nombre : "DESCONOCIDA")
                     << " | " << c->distancia << " km | costo: " << c->costo << "\n";
                hay = true;
            }
            c = c->sig;
        }
        p = p->sig;
    }
    if (!hay) cout << "No hay conexiones registradas.\n";
}

void menuConexiones() {
    int op;
    do {
        cout << "\n=== GESTION DE CONEXIONES ===\n"
             << "1. Crear conexion\n2. Listar conexiones\n3. Eliminar conexion\n0. Volver\n";
        op = leerEntero("Opcion: ");
        switch (op) {
            case 1: agregarConexion(); break;
            case 2: listarConexiones(); break;
            case 3: eliminarConexion(); break;
            case 0: break;
            default: cout << "Opcion invalida.\n";
        }
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Pasajeros: lista enlazada ----------

void agregarPasajero() {
    int id = leerEntero("ID del pasajero: ");
    if (id <= 0 || buscarPasajero(id) != NULL) {
        cout << "ID invalido o repetido.\n"; return;
    }
    string nombre = leerTexto("Nombre: ");
    int edad = leerEntero("Edad: ");
    int ciudadId = leerEntero("ID de ciudad asociada (0 si no tiene): ");
    if (nombre.empty() || edad < 0 || (ciudadId != 0 && buscarCiudad(ciudadId) == NULL)) {
        cout << "Datos invalidos. No se registro.\n"; return;
    }
    Pasajero *n = new Pasajero;
    n->id = id; n->nombre = nombre; n->edad = edad; n->ciudadId = ciudadId;
    n->sig = primerPasajero; primerPasajero = n;
    registrarAccion("Se registro pasajero " + nombre);
    cout << "Pasajero registrado.\n";
}

void listarPasajeros() {
    if (primerPasajero == NULL) { cout << "No hay pasajeros.\n"; return; }
    Pasajero *p = primerPasajero;
    while (p != NULL) {
        Ciudad *c = buscarCiudad(p->ciudadId);
        cout << "ID: " << p->id << " | " << p->nombre << " | Edad: " << p->edad
             << " | Ciudad: " << (p->ciudadId == 0 ? "Sin asignar" : (c ? c->nombre : "No disponible")) << "\n";
        p = p->sig;
    }
}

void modificarPasajero() {
    int id = leerEntero("ID del pasajero a modificar: ");
    Pasajero *p = buscarPasajero(id);
    if (p == NULL) { cout << "Pasajero no encontrado.\n"; return; }
    string nombre = leerTexto("Nuevo nombre: ");
    int edad = leerEntero("Nueva edad: ");
    int ciudadId = leerEntero("Nuevo ID de ciudad (0 si no tiene): ");
    if (nombre.empty() || edad < 0 || (ciudadId != 0 && buscarCiudad(ciudadId) == NULL)) {
        cout << "Datos invalidos. No se modifico el pasajero.\n"; return;
    }
    p->nombre = nombre;
    p->edad = edad;
    p->ciudadId = ciudadId;
    registrarAccion("Se modifico pasajero ID " + numeroATexto(id));
    cout << "Pasajero modificado correctamente.\n";
}

void eliminarPasajero() {
    int id = leerEntero("ID del pasajero a eliminar: ");
    Pasajero *p = primerPasajero, *ant = NULL;
    while (p != NULL && p->id != id) { ant = p; p = p->sig; }
    if (p == NULL) { cout << "Pasajero no encontrado.\n"; return; }
    if (ant == NULL) primerPasajero = p->sig;
    else ant->sig = p->sig;

    // Eliminar solicitudes pendientes del pasajero eliminado.
    Solicitud *s = frente, *prev = NULL;
    while (s != NULL) {
        if (s->pasajeroId == id) {
            Solicitud *b = s;
            if (prev == NULL) frente = s->sig;
            else prev->sig = s->sig;
            s = s->sig;
            if (b == finalCola) finalCola = prev;
            delete b;
        } else { prev = s; s = s->sig; }
    }
    string nombre = p->nombre;
    delete p;
    registrarAccion("Se elimino pasajero " + nombre);
    cout << "Pasajero y sus solicitudes pendientes eliminados.\n";
}

void menuPasajeros() {
    int op;
    do {
        cout << "\n=== GESTION DE PASAJEROS ===\n"
             << "1. Registrar\n2. Listar\n3. Modificar\n4. Eliminar\n0. Volver\n";
        op = leerEntero("Opcion: ");
        switch (op) {
            case 1: agregarPasajero(); break;
            case 2: listarPasajeros(); break;
            case 3: modificarPasajero(); break;
            case 4: eliminarPasajero(); break;
            case 0: break;
            default: cout << "Opcion invalida.\n";
        }
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Cola de solicitudes FIFO ----------

void encolarSolicitud() {
    int pasajero = leerEntero("ID pasajero: ");
    int origen = leerEntero("ID ciudad origen: ");
    int destino = leerEntero("ID ciudad destino: ");
    if (buscarPasajero(pasajero) == NULL || buscarCiudad(origen) == NULL ||
        buscarCiudad(destino) == NULL || origen == destino) {
        cout << "Error: pasajero/ciudades invalidos.\n"; return;
    }
    Solicitud *n = new Solicitud;
    n->pasajeroId = pasajero; n->origenId = origen; n->destinoId = destino; n->sig = NULL;
    if (finalCola == NULL) frente = finalCola = n;
    else { finalCola->sig = n; finalCola = n; }
    registrarAccion("Se agrego solicitud a la cola");
    cout << "Solicitud agregada al final de la cola.\n";
}

void atenderSolicitud() {
    if (frente == NULL) { cout << "La cola esta vacia.\n"; return; }
    Solicitud *b = frente;
    Pasajero *p = buscarPasajero(b->pasajeroId);
    Ciudad *o = buscarCiudad(b->origenId);
    Ciudad *d = buscarCiudad(b->destinoId);
    cout << "Atendiendo solicitud: " << (p ? p->nombre : "Pasajero")
         << " | " << (o ? o->nombre : "?") << " -> " << (d ? d->nombre : "?") << "\n";
    frente = frente->sig;
    if (frente == NULL) finalCola = NULL;
    delete b;
    registrarAccion("Se atendio una solicitud");
}

void listarSolicitudes() {
    if (frente == NULL) { cout << "La cola esta vacia.\n"; return; }
    Solicitud *s = frente;
    int posicion = 1;
    while (s != NULL) {
        Pasajero *p = buscarPasajero(s->pasajeroId);
        Ciudad *o = buscarCiudad(s->origenId);
        Ciudad *d = buscarCiudad(s->destinoId);
        cout << posicion++ << ". " << (p ? p->nombre : "?") << " | "
             << (o ? o->nombre : "?") << " -> " << (d ? d->nombre : "?") << "\n";
        s = s->sig;
    }
}

void menuCola() {
    int op;
    do {
        cout << "\n=== COLA DE SOLICITUDES (FIFO) ===\n"
             << "1. Crear solicitud\n2. Atender primera solicitud\n3. Listar solicitudes\n0. Volver\n";
        op = leerEntero("Opcion: ");
        switch (op) {
            case 1: encolarSolicitud(); break;
            case 2: atenderSolicitud(); break;
            case 3: listarSolicitudes(); break;
            case 0: break;
            default: cout << "Opcion invalida.\n";
        }
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Pila de historial LIFO ----------

void verUltimaAccion() {
    if (topeHistorial == NULL) { cout << "El historial esta vacio.\n"; return; }
    cout << "Ultima accion: " << topeHistorial->descripcion << "\n";
}

void retirarAccion() {
    if (topeHistorial == NULL) { cout << "El historial esta vacio.\n"; return; }
    Accion *b = topeHistorial;
    cout << "Retirando del historial: " << b->descripcion << "\n";
    topeHistorial = topeHistorial->sig;
    delete b;
}

void listarHistorial() {
    if (topeHistorial == NULL) { cout << "El historial esta vacio.\n"; return; }
    Accion *p = topeHistorial;
    int i = 1;
    while (p != NULL) { cout << i++ << ". " << p->descripcion << "\n"; p = p->sig; }
}

void menuPila() {
    int op;
    do {
        cout << "\n=== PILA DE HISTORIAL (LIFO) ===\n"
             << "1. Ver ultima accion\n2. Retirar accion del historial\n3. Listar historial\n0. Volver\n";
        op = leerEntero("Opcion: ");
        switch (op) {
            case 1: verUltimaAccion(); break;
            case 2: retirarAccion(); break;
            case 3: listarHistorial(); break;
            case 0: break;
            default: cout << "Opcion invalida.\n";
        }
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Recorridos de grafo y rutas ----------

void bfs(int inicio) {
    if (buscarCiudad(inicio) == NULL) { cout << "Ciudad inicial inexistente.\n"; return; }
    int n = contarCiudades();
    int *cola = new int[n];
    int *visitado = new int[n];
    for (int i = 0; i < n; i++) visitado[i] = 0;
    int ini = 0, fin = 0;
    cola[fin++] = inicio;
    visitado[fin-1] = 1;
    cout << "BFS: ";
    while (ini < fin) {
        int id = cola[ini++];
        Ciudad *p = buscarCiudad(id);
        cout << (p ? p->nombre : "?") << " ";
        Conexion *c = p->adyacentes;
        while (c != NULL) {
            bool visto = false;
            for (int j = 0; j < fin; j++) if (cola[j] == c->destino) visto = true;
            if (!visto && fin < n) cola[fin++] = c->destino;
            c = c->sig;
        }
    }
    cout << "\n";
    delete [] cola;
    delete [] visitado;
}

void dfsRecursivo(int id, int *visitados, int &cantidad) {
    for (int i = 0; i < cantidad; i++) if (visitados[i] == id) return;
    visitados[cantidad++] = id;
    Ciudad *p = buscarCiudad(id);
    if (p == NULL) return;
    cout << p->nombre << " ";
    Conexion *c = p->adyacentes;
    while (c != NULL) { dfsRecursivo(c->destino, visitados, cantidad); c = c->sig; }
}

void ejecutarDFS(int inicio) {
    if (buscarCiudad(inicio) == NULL) { cout << "Ciudad inicial inexistente.\n"; return; }
    int n = contarCiudades();
    int *visitados = new int[n];
    int cantidad = 0;
    cout << "DFS: ";
    dfsRecursivo(inicio, visitados, cantidad);
    cout << "\n";
    delete [] visitados;
}

void dijkstra() {
    if (primeraCiudad == NULL) { cout << "No hay ciudades.\n"; return; }
    int origen = leerEntero("ID ciudad origen: ");
    int destino = leerEntero("ID ciudad destino: ");
    if (buscarCiudad(origen) == NULL || buscarCiudad(destino) == NULL) {
        cout << "Origen o destino inexistente.\n"; return;
    }

    int n = contarCiudades();
    Ciudad **ciudades = new Ciudad*[n];
    double *dist = new double[n];
    int *visitado = new int[n];
    int *anterior = new int[n];

    Ciudad *p = primeraCiudad;
    for (int i = 0; i < n; i++) {
        ciudades[i] = p; p = p->sig;
        dist[i] = 1e100; visitado[i] = 0; anterior[i] = -1;
    }

    int indiceOrigen = -1, indiceDestino = -1;
    for (int i = 0; i < n; i++) {
        if (ciudades[i]->id == origen) indiceOrigen = i;
        if (ciudades[i]->id == destino) indiceDestino = i;
    }
    dist[indiceOrigen] = 0;

    for (int paso = 0; paso < n; paso++) {
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!visitado[i] && (u == -1 || dist[i] < dist[u])) u = i;

        if (u == -1 || dist[u] >= 1e99) break;
        visitado[u] = 1;

        Conexion *c = ciudades[u]->adyacentes;
        while (c != NULL) {
            int v = -1;
            for (int j = 0; j < n; j++) if (ciudades[j]->id == c->destino) v = j;
            if (v != -1 && !visitado[v] && dist[u] + c->distancia < dist[v]) {
                dist[v] = dist[u] + c->distancia;
                anterior[v] = u;
            }
            c = c->sig;
        }
    }

    if (dist[indiceDestino] >= 1e99) {
        cout << "No existe una ruta entre esas ciudades.\n";
    } else {
        int *ruta = new int[n];
        int cantidad = 0;
        int actual = indiceDestino;
        while (actual != -1 && cantidad < n) {
            ruta[cantidad++] = ciudades[actual]->id;
            actual = anterior[actual];
        }
        cout << "Ruta de menor distancia: ";
        for (int i = cantidad - 1; i >= 0; i--) {
            Ciudad *c = buscarCiudad(ruta[i]);
            cout << (c ? c->nombre : "?");
            if (i > 0) cout << " -> ";
        }
        cout << "\nDistancia total: " << dist[indiceDestino] << " km\n";
        delete [] ruta;
    }

    delete [] ciudades;
    delete [] dist;
    delete [] visitado;
    delete [] anterior;
}

void menuRutas() {
    int op;
    do {
        cout << "\n=== RUTAS Y ALGORITMOS ===\n"
             << "1. Recorrido BFS\n2. Recorrido DFS\n3. Ruta mas corta (Dijkstra por distancia)\n0. Volver\n";
        op = leerEntero("Opcion: ");
        if (op == 1) bfs(leerEntero("ID ciudad inicial: "));
        else if (op == 2) ejecutarDFS(leerEntero("ID ciudad inicial: "));
        else if (op == 3) dijkstra();
        else if (op != 0) cout << "Opcion invalida.\n";
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Reportes ----------

void reportes() {
    cout << "\n=== REPORTES DEL SISTEMA ===\n";
    cout << "Ciudades registradas: " << contarCiudades() << "\n";
    cout << "Conexiones bidireccionales: " << contarConexiones() << "\n";
    cout << "Pasajeros registrados: " << contarPasajeros() << "\n";
    int solicitudes = 0;
    Solicitud *s = frente;
    while (s != NULL) { solicitudes++; s = s->sig; }
    cout << "Solicitudes pendientes: " << solicitudes << "\n";
    int acciones = 0;
    Accion *a = topeHistorial;
    while (a != NULL) { acciones++; a = a->sig; }
    cout << "Acciones en historial: " << acciones << "\n";
    if (primeraCiudad != NULL) {
        Ciudad *mayor = primeraCiudad;
        Ciudad *p = primeraCiudad->sig;
        while (p != NULL) {
            if (p->poblacion > mayor->poblacion) mayor = p;
            p = p->sig;
        }
        cout << "Ciudad con mayor poblacion registrada: " << mayor->nombre
             << " (" << mayor->poblacion << " habitantes)\n";
    }
}

// ---------- Guardar y cargar ----------

void guardarDatos() {
    string nombre = leerTexto("Nombre del archivo (ejemplo datos.txt): ");
    if (nombre.empty()) { cout << "Nombre invalido.\n"; return; }
    ofstream archivo(nombre.c_str());
    if (!archivo) { cout << "No se pudo abrir el archivo para escritura.\n"; return; }

    int n = contarCiudades();
    archivo << n << "\n";
    Ciudad *c = primeraCiudad;
    while (c != NULL) {
        archivo << c->id << "\n" << c->nombre << "\n" << c->departamento << "\n" << c->poblacion << "\n";
        c = c->sig;
    }

    int m = contarConexiones();
    archivo << m << "\n";
    c = primeraCiudad;
    while (c != NULL) {
        Conexion *e = c->adyacentes;
        while (e != NULL) {
            if (c->id < e->destino)
                archivo << c->id << " " << e->destino << " " << e->distancia << " " << e->costo << "\n";
            e = e->sig;
        }
        c = c->sig;
    }

    archivo << contarPasajeros() << "\n";
    Pasajero *p = primerPasajero;
    while (p != NULL) {
        archivo << p->id << "\n" << p->nombre << "\n" << p->edad << "\n" << p->ciudadId << "\n";
        p = p->sig;
    }

    int totalSolicitudes = 0;
    Solicitud *s = frente;
    while (s != NULL) { totalSolicitudes++; s = s->sig; }
    archivo << totalSolicitudes << "\n";
    s = frente;
    while (s != NULL) {
        archivo << s->pasajeroId << " " << s->origenId << " " << s->destinoId << "\n";
        s = s->sig;
    }

    archivo.close();
    cout << "Datos guardados correctamente en " << nombre << ".\n";
}

void liberarSolicitudes() {
    while (frente != NULL) {
        Solicitud *b = frente; frente = frente->sig; delete b;
    }
    finalCola = NULL;
}

void cargarDatos() {
    string nombre = leerTexto("Nombre del archivo a cargar: ");
    ifstream archivo(nombre.c_str());
    if (!archivo) { cout << "No se pudo abrir el archivo.\n"; return; }

    // Se carga primero en memoria actual despues de limpiar. Si el formato falla,
    // se informa el error; por eso se recomienda cargar archivos generados por el sistema.
    // Liberar ciudades y aristas.
    while (primeraCiudad != NULL) {
        Ciudad *b = primeraCiudad;
        primeraCiudad = primeraCiudad->sig;
        Conexion *e = b->adyacentes;
        while (e != NULL) { Conexion *x = e; e = e->sig; delete x; }
        delete b;
    }
    while (primerPasajero != NULL) {
        Pasajero *b = primerPasajero; primerPasajero = primerPasajero->sig; delete b;
    }
    liberarSolicitudes();
    liberarArbol(raiz); raiz = NULL;

    int n;
    if (!(archivo >> n) || n < 0 || n > 100000) { cout << "Archivo invalido.\n"; return; }
    archivo.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 0; i < n; i++) {
        int id, poblacion;
        string nom, dep;
        if (!(archivo >> id)) { cout << "Archivo incompleto.\n"; return; }
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(archivo, nom);
        getline(archivo, dep);
        if (!(archivo >> poblacion)) { cout << "Archivo incompleto.\n"; return; }
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');
        if (id <= 0 || buscarCiudad(id) != NULL || poblacion < 0 || nom.empty() || dep.empty()) {
            cout << "Datos de ciudad invalidos en archivo.\n"; return;
        }
        Ciudad *c = new Ciudad;
        c->id = id; c->nombre = nom; c->departamento = dep;
        c->poblacion = poblacion; c->adyacentes = NULL;
        c->sig = primeraCiudad; primeraCiudad = c;
    }
    reconstruirArbol();

    int m;
    if (!(archivo >> m) || m < 0 || m > 1000000) { cout << "Seccion de conexiones invalida.\n"; return; }
    for (int i = 0; i < m; i++) {
        int a, b; double d, costo;
        if (!(archivo >> a >> b >> d >> costo)) { cout << "Conexion incompleta.\n"; return; }
        Ciudad *ca = buscarCiudad(a), *cb = buscarCiudad(b);
        if (ca == NULL || cb == NULL || a == b || d < 0 || costo < 0 || existeConexion(a,b)) {
            cout << "Conexion invalida en archivo.\n"; return;
        }
        insertarArista(ca, b, d, costo);
        insertarArista(cb, a, d, costo);
    }

    int np;
    if (!(archivo >> np) || np < 0 || np > 1000000) { cout << "Seccion de pasajeros invalida.\n"; return; }
    archivo.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < np; i++) {
        int id, edad, ciudadId;
        string nom;
        if (!(archivo >> id)) { cout << "Pasajero incompleto.\n"; return; }
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(archivo, nom);
        if (!(archivo >> edad >> ciudadId)) { cout << "Pasajero incompleto.\n"; return; }
        if (id <= 0 || buscarPasajero(id) != NULL || edad < 0 ||
            (ciudadId != 0 && buscarCiudad(ciudadId) == NULL) || nom.empty()) {
            cout << "Pasajero invalido en archivo.\n"; return;
        }
        Pasajero *p = new Pasajero;
        p->id = id; p->nombre = nom; p->edad = edad; p->ciudadId = ciudadId;
        p->sig = primerPasajero; primerPasajero = p;
    }

    int ns;
    if (!(archivo >> ns) || ns < 0 || ns > 1000000) { cout << "Seccion de solicitudes invalida.\n"; return; }
    for (int i = 0; i < ns; i++) {
        int pas, ori, des;
        if (!(archivo >> pas >> ori >> des)) { cout << "Solicitud incompleta.\n"; return; }
        if (buscarPasajero(pas) == NULL || buscarCiudad(ori) == NULL ||
            buscarCiudad(des) == NULL || ori == des) {
            cout << "Solicitud invalida en archivo.\n"; return;
        }
        Solicitud *s = new Solicitud;
        s->pasajeroId = pas; s->origenId = ori; s->destinoId = des; s->sig = NULL;
        if (finalCola == NULL) frente = finalCola = s;
        else { finalCola->sig = s; finalCola = s; }
    }

    archivo.close();
    registrarAccion("Se cargaron datos desde " + nombre);
    cout << "Datos cargados correctamente.\n";
}

void menuArchivos() {
    int op;
    do {
        cout << "\n=== GUARDAR / CARGAR ===\n1. Guardar datos\n2. Cargar datos\n0. Volver\n";
        op = leerEntero("Opcion: ");
        if (op == 1) guardarDatos();
        else if (op == 2) cargarDatos();
        else if (op != 0) cout << "Opcion invalida.\n";
        if (op != 0) pausar();
    } while (op != 0);
}

// ---------- Liberacion total de memoria ----------

void liberarTodo() {
    while (primeraCiudad != NULL) {
        Ciudad *b = primeraCiudad; primeraCiudad = primeraCiudad->sig;
        Conexion *c = b->adyacentes;
        while (c != NULL) { Conexion *x = c; c = c->sig; delete x; }
        delete b;
    }
    while (primerPasajero != NULL) {
        Pasajero *b = primerPasajero; primerPasajero = primerPasajero->sig; delete b;
    }
    liberarSolicitudes();
    while (topeHistorial != NULL) {
        Accion *b = topeHistorial; topeHistorial = topeHistorial->sig; delete b;
    }
    liberarArbol(raiz); raiz = NULL;
}

int main() {
    int opcion;
    do {
        cout << "\n============================================\n"
             << "   SISTEMA DE GESTION DE TRANSPORTE\n"
             << "============================================\n"
             << "1. Gestion de ciudades\n"
             << "2. Gestion de conexiones\n"
             << "3. Gestion de pasajeros\n"
             << "4. Cola de solicitudes\n"
             << "5. Pila de historial\n"
             << "6. Arbol de busquedas\n"
             << "7. Rutas y algoritmos\n"
             << "8. Reportes\n"
             << "9. Guardar / cargar\n"
             << "0. Salir\n";
        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {
            case 1: menuCiudades(); break;
            case 2: menuConexiones(); break;
            case 3: menuPasajeros(); break;
            case 4: menuCola(); break;
            case 5: menuPila(); break;
            case 6: {
                int id = leerEntero("ID de ciudad para buscar en el arbol: ");
                if (buscarEnArbol(id)) cout << "El ID " << id << " existe en el arbol.\n";
                else cout << "El ID no esta en el arbol.\n";
                pausar();
                break;
            }
            case 7: menuRutas(); break;
            case 8: reportes(); pausar(); break;
            case 9: menuArchivos(); break;
            case 0: cout << "Cerrando sistema...\n"; break;
            default: cout << "Opcion invalida.\n"; pausar();
        }
    } while (opcion != 0);

    liberarTodo();
    cout << "Memoria liberada. Hasta pronto.\n";
    return 0;
}
