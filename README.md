# Proyecto1-BD

## Introducción

Este proyecto tiene la finalidad de crear un gestor dado un dataset y realizar operaciones con ayuda de la organización de archivos.

## Objetivo principal

- El objetivo principal del proyecto es comprender y aplicar técnicas de almacenamiento de archivos físicos en un gestor de base de datos para un dataset.

## Objetivos secundarios

- Implementar tres técnicas de organización de archivos en memoria secundaria.
- Implementación de un parser SQL a secuencias ejecutables.
- Creación de GUI amigable para el usuario.

## Descripción del dominio de datos

Hemos usado una base de datos obtenida de Kaggle que cuenta con información sobre 11000 libros. Se eligió este dominio de datos por su fácil implementación con nuestras estructuras y por contar con columnas de datos únicos, como puede ser el ISBN, title, average rating, etc.

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

![](https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.sahinarslan.tech%2Fposts%2Fdeep-dive-into-data-structures-using-javascript-avl-tree%2F&psig=AOvVaw007yMgl7PRLIVosJao1AiX&ust=1695734583950000&source=images&cd=vfe&opi=89978449&ved=0CBAQjRxqFwoTCJCg-d7txYEDFQAAAAAdAAAAABAD)

n es el número de nodos en el AVL File y k el número de nodos en el rango que se busca 

| Command | Description | Complexity |
| --- | --- | --- |
| Add | Se desciende en el árbol y se ubica adecuadamente el registro; luego, se verifica si el árbol está balanceado. Si no lo está, se realizan rotaciones.| O(logn) |
| Search | Se desciende en el árbol. Si se logra ubicar en un registro con igual key a la buscada se retorna true, sino false. | O(logn) |
| RangeSearch | Se desciende recursivamente el subárbol izquierdo y derecho y se almacenan los nodos que se encuentren dentro del rango en el vector Records. | O(k + logn) |
| Remove | Se desciende en el árbol buscando la key que se desee eliminar y la cual se ha pasado como parámetro, si se encuentra, se realiza la eliminación y si no se encuentra retorna false. | O(logn) |

## Extendible Hashing

Creación de buckets con código hash para almacenar registros en la memoria secundaria. Admite Insert, RangeSearch y Remove.

| Command | Description | Complexity |
| --- | --- | --- |
| Insert | Se maneja la inserción del Extendible Hashing, además gestiona el desbordamiento de los Buckets cuando es necesario.| O(1) const O(n) desbordamiento |
| Search | Se busca el registro a partir de su key en el hash. | O(1) |
| RangeSearch | La función Search busca un registro específico y la función RangeSearch busca los registros en un rango de keys y los retorna. | O(1) const O(n) desbordamiento |
| Remove | Se calcula la posición del bucket en la que se debe buscar el key y se realiza una búsqueda y si se encuentra el registro se elimina. | O(1) const O(n) desbordamiento |

## Sequential File

Técnica que almacena los nuevos records en un espacio auxiliar, que será reestructurado una vez este pase un cierto límite de espacio. Aprovecha que los records principales están ordenados bajo el key para realizar operaciones en tiempo logarítmico.

| Command | Description | Complexity |
| --- | --- | --- |
| InsertRecord | La inserción en el espacio auxiliar siempre será constante, sin embargo, si no hay espacio, se tendrá que reestructurar el archivo en tiempo lineal. | Principal: O(n), Auxiliar: O(1) |
| SearchRecord | Al estar el archivo principal ordenado, podemos usar búsqueda binaria para encontrar el record en tiempo logarítmico, mientras que si está en el archivo auxiliar, esto se convertirá en tiempo lineal | Principal: O(lgn), Auxiliar: O(n) |
| RangeSearchRecords | Al igual que la búsqueda específica, la búsqueda por rango demorará tiempo logarítmico si el record se encuentra en el espacio principal o lineal si está en el auxiliar. Después de esto, se podrá encontrar los siguientes records dentro del rango en tiempo lineal. | Principal: O(n), Auxiliar: O(n) |

## SQL Parser

Definiciones:
- Token: Grupo de caracteres con un significado (reserved_words, numbers, table_name)
- Statement: Es un tipo de consulta (CREATE, SELECT, INSERT, DELETE)
- Parser: Lee los tokens de un statement y retorna un objeto con cafacterísticas importante sobre la consulta.
- Match(X): Verifica si el token actual es igual X.

Se definieron varias clases para cada tipo de Statement: CreateTableStatement, SelectStatement, InsertStatement y DeleteStatement. Cada una de estas clases contiene atributos que almacenan información relevante para la sentencia SQL correspondiente, como el nombre de la tabla, los valores a insertar, las condiciones de selección, etc.

En primer lugar, se llama al método parseSQLStatement() que lee el primer token de la secuencia y determina el tipo de sentencia SQL que se está analizando. Luego, invoca un método específico para analizar esa sentencia en particular: parseCreateTable(), parseSelect(), parseInsert() o parseDelete().

Cada uno de estos métodos lee la consulta y por cada token obtenido va haciendo match hasta llegar al final de la consulta donde retorna el objeto Statement.

A continuación se mostrará las consultas que soporta nuestro parser.

| Consulta | Query |
| CREATE | create table books_avl from file "data.csv" using index avl("bookID"); |
| INSERT | insert into books_avl values(55555, Libro, Josue/Gabriel/Nico/Max, 4.2); |
| SELECT equals | select * from books_avl where bookID = 2 |
| SELECT between | select * from books_avl where bookID between 1 and 20; |
| DELETE | delete from books_avl where bookID = 1; |

Reserved words:
- create
- select
- insert
- delete
- from
- where
- values
- and
- between
- table
- *
- using
- file
- into
- index

## Resultados experimentales
CREATE:
| Índice | Tiempo de ejecución (ms) | Accesos a disco duro |
| AVL | 2448700 | O(n) |
| Extendible Hash | 5931850 | O(n) |
| Sequential | 143483000 | O(n) |

SELECT equals:
| Índice | Tiempo de ejecución (ms) | Accesos a disco duro |
|  AVL | 2557 | O(lgn) |
| Extendible Hash | 132  | O(1) |
| Sequential | 1588 | O(logn + k) |

SELECT between:
| Índice | Tiempo de ejecución (ms) | Accesos a disco duro |
| AVL | 59690 | O(n) |
| Sequential | 15415 | O(logn) |

INSERT:
| Índice | Tiempo de ejecución (ms) | Accesos a disco duro |
| AVL | 358 | O(lgn)  |

DELETE:
| Índice | Tiempo de ejecución (ms) | Accesos a disco duro |

## Análisis de resultados y conclusiones
