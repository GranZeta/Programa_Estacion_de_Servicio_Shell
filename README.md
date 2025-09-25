# Sistema de Gasolinera (C++)

Este proyecto implementa un sistema de gestión de gasolinera con **archivos binarios**, **estructuras**, **listas dinámicas** y **POO** en C++.

## Funcionalidades principales
- Cargar, listar, modificar y desactivar productos.
- Registrar, listar y anular ventas.
- Reportes de recaudación total y por producto (ordenados).
- Pasar ventas a lista enlazada y mostrarlas.

## Arquitectura
- Todo está encapsulado en la clase `SistemaGasolinera`.
- Uso de archivos binarios `productos.dat` y `ventas.dat` para persistencia.
- El `main` solo instancia el sistema y ejecuta el menú.

## Uso
1. Compilar:
   ```bash
   g++ sistema_gasolinera.cpp -o sistema
   ```
2. Ejecutar:
   ```bash
   ./sistema
   ```

## Archivos generados
- `productos.dat`: almacena productos (código, nombre, precio, activo).
- `ventas.dat`: almacena ventas (ticket, producto, cantidad, total, vendedor).
