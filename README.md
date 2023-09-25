# Proyecto1-BD

## Introducción

Este proyecto tiene la finalidad de crear un gestor dado un dataset y realizar operaciones con ayuda de la organización de archivos.

## Objetivo principal

- El objetivo principal del proyecto es Comprender y aplicar técnicas de almacenamiento de archivos físicos.

## Objetivos secundarios

- Implementar tres técnicas de organización de archivos en memoria secundaria.
- Implementación de un parser SQL a secuencias ejecutables.
- Creación de GUI amigable para el usuario.


## Descripción del dominio de datos

Hemos usado una base de datos obtenida de Kaggle que cuenta con información sobre 11 000 libros. Se eligió este dominio de datos por su fácil implementación con nuestras estructuras y por contar con columnas de datos únicos, como puede ser el ISBN, title, average rating, etc.

Atributos:

- bookID: Identificador único de un libro.
- title: Titulo del libro.
- authors: Autores del libro.
- average_rating: Calificación promedio del libro

## Resultados esperados

Se espera que las operaciones básicas que se puedan realizar en una base de datos se ejecuten en poco tiempo de modo que se aproveche los índices que hemos creado. Además, se espera que estas mantengan su rendimiento aún cuando la cantidad de datos sigue en crecimiento.

## Técnicas de indexación

Para nuestro proyecto, realizamos las siguientes tres estructuras de datos:

- AVL File
- Extendible Hashing
- Sequential File

## AVL File

El AVL File es la implementación de el árbol de búsqueda binaria auto balanceado que funciona para almacenar registros de memoria secundaria. Nuestra implementación admite insert, range search y remove.

n es el número de nodos en el AVL File y k el número de nodos en el rango que se busca 

| Command | Description | Complexity |
| --- | --- | --- |
| Insert | Se desciende en el árbol y se ubica adecuadamente el registro; luego, se verifica si el árbol está balanceado. Si no lo está, se realizan rotaciones.| O(logn) |
| RangeSearch | Se desciende recursivamente el subárbol izquierdo y derecho y se almacenan los nodos que se encuentren dentro del rango en el vector Records. | O(k + logn) |
| Remove | Se desciende en el árbol buscando la key que se desee eliminar y la cual se ha pasado como parámetro, si se encuentra, se realiza la eliminación y si no se encuentra retorna false. | O(logn) |

## Extendible Hashing

Creación de buckets con código hash para almacenar registros en la memoria secundaria. Admite Insert, RangeSearch y Remove.

| Command | Description | Complexity |
| --- | --- | --- |
| Insert | Se maneja la inserción del Extendible Hashing, además gestiona el desbordamiento de los Buckets cuando es necesario.| O(1) const O(n) desbordamiento |
| RangeSearch | La función Search busca un registro específico y la función RangeSearch busca los registros en un rango de keys y los retorna. | O(1) const O(n) desbordamiento |
| Remove | Se calcula la posición del bucket en la que se debe buscar el key y se realiza una búsqueda y si se encuentra el registro se elimina. | O(1) const O(n) desbordamiento |

## Sequential File

En esta técnica el archivo se almacena uno tras otro de forma secuancial. Esta implementación admite Insert y search.

| Command | Description | Complexity |
| --- | --- | --- |
| Insert | Si hay espacio en el archivo auxiliar, se busca la posición adecuada y se inserta. Si no hay espacio en el archivo auxiliar se reorganiza el archivo y se intenta nuevamente la iserción.| O(n) |
| RangeSearch | En el Sequential File al estar ordenado los registros es más conveniente realizar la búsqueda binaria para realizar una búsqueda específica o por rangos. | O(log n) |
