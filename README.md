# Sistema-Gestion-Transporte

Descripción
Aplicación de consola desarrollada en C++ para administrar ciudades, conexiones entre ellas, pasajeros y solicitudes de viaje. El sistema permite consultar recorridos y calcular la ruta de menor distancia entre dos ciudades.
Objetivo
Aplicar estructuras de datos dinámicas y algoritmos de recorridos sobre grafos para resolver operaciones básicas de un sistema de transporte.
Funcionalidades
1. Gestión de ciudades: registrar, listar, buscar, modificar y eliminar ciudades.
2. Gestión de conexiones: crear y administrar conexiones entre ciudades, indicando distancia y costo.
3. Gestión de pasajeros: registrar, listar, modificar y eliminar pasajeros.
4. Cola de solicitudes: crear, listar y atender solicitudes en orden FIFO.
5. Pila de historial: consultar y listar acciones, y retirar la acción más reciente (LIFO).
6. Árbol binario de búsqueda: buscar ciudades por su identificador.
7. Rutas y algoritmos: recorrer el grafo mediante BFS y DFS, y calcular la ruta de menor distancia con Dijkstra.
8. Reportes: consultar información general del sistema.
9. Guardar y cargar: almacenar y recuperar los datos mediante archivos de texto.
Estructuras de datos utilizadas
- Lista simplemente enlazada: almacena ciudades y pasajeros.
- Grafo no dirigido con listas de adyacencia: representa las conexiones entre ciudades. Cada conexión se almacena en ambos sentidos.
- Cola enlazada (FIFO): atiende primero la solicitud que llegó primero.
- Pila enlazada (LIFO): permite consultar y retirar la acción más reciente del historial.
- Árbol binario de búsqueda: organiza los identificadores de las ciudades para realizar búsquedas.
- Arreglos auxiliares: se utilizan durante algunos recorridos y cálculos de rutas.
Algoritmos
- BFS (búsqueda en anchura): visita las ciudades por niveles desde una ciudad inicial.
- DFS (búsqueda en profundidad): explora un camino del grafo antes de retroceder.
- Dijkstra: calcula la ruta de menor distancia. La implementación utiliza selección lineal del siguiente vértice, por lo que su complejidad es aproximadamente O(V² + E) en esta representación.
Requisitos
- Compilador de C++.
- Compatible con compilación en estándar C++98.
- Dev-C++ u otro entorno que permita compilar archivos .cpp.
Compilación y ejecución
1. Abrir sistema_transporte.cpp en Dev-C++.
2. Seleccionar Compilar y ejecutar.
3. Seguir las opciones del menú principal.
4. Para guardar información, entrar en Guardar / cargar y utilizar la opción de guardar.
5. Para recuperar información, utilizar la opción de cargar e indicar el nombre del archivo.
Prueba básica sugerida
Registrar estas ciudades:
- ID 1: Villavicencio, Meta.
- ID 2: Restrepo, Meta.
- ID 3: Cumaral, Meta.
Crear estas conexiones:
- Villavicencio–Restrepo: 18 km.
- Restrepo–Cumaral: 25 km.
- Villavicencio–Cumaral: 40 km.
Al ejecutar Dijkstra desde la ciudad 1 hasta la ciudad 3, la ruta de menor distancia debe ser de 40 km, directamente entre Villavicencio y Cumaral. Si se desea comprobar la ruta por Restrepo, se puede cambiar la conexión directa a un valor mayor que 43 km; en ese caso, el recorrido por Restrepo será el más corto.
Consideraciones
- Los identificadores de ciudades y pasajeros deben ser válidos y no repetirse.
- Las conexiones requieren que las dos ciudades estén registradas.
- Dijkstra trabaja con distancias no negativas.
- El historial registra acciones, pero retirar una acción de la pila no deshace la operación que la originó.
- Se recomienda cargar archivos generados por el propio programa y conservar una copia de seguridad de los datos.
Cierre
El desarrollo permitió implementar y relacionar diferentes estructuras de datos dinámicas en una aplicación funcional. Se integraron operaciones de inserción, consulta, modificación y eliminación, además de recorridos de grafos y búsqueda de rutas. Como resultado, se fortaleció la comprensión del manejo de memoria mediante punteros, la organización de información y el uso de algoritmos para resolver problemas de conectividad entre ciudades.
