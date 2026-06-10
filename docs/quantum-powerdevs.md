# Quantum PowerDEVS design notes

Este documento deja registrada la arquitectura de la libreria cuantica para
PowerDEVS, las decisiones tomadas, los problemas encontrados y las reglas de
trabajo para extenderla. Esta escrito como memoria tecnica del estado actual del
codigo, no como una especificacion ideal futura.

## Objetivo

El objetivo de la libreria es permitir armar circuitos cuanticos desde la UI de
PowerDEVS usando modelos atomicos visuales:

- fuentes de qubits iniciales (`Q Zero`, `Q One`);
- compuertas (`H`, `X`, `Y`, `Z`, `ID`, `CNOT`);
- medicion (`Measure`);
- bloques clasicos de conteo o graficacion (`Event Counter`, `Q Count`,
  `GnuPlot`).

La regla de uso buscada es:

```text
Entrar a Quantum Circuit
Colocar Quantum Core
Colocar fuentes, compuertas y mediciones
Conectar cables
Ejecutar
```

El usuario no deberia tener que pasar manualmente un `StateVector` entre bloques
ni crear modelos auxiliares para sincronizar el circuito. Esa responsabilidad
queda dentro de la libreria.

## Formalismo DEVS minimo

Un modelo atomico DEVS se puede describir como:

```text
M = <X, S, Y, delta_int, delta_ext, lambda, ta>
```

Donde:

- `X` es el conjunto de eventos de entrada.
- `S` es el estado interno del modelo.
- `Y` es el conjunto de eventos de salida.
- `delta_int` cambia el estado cuando vence el tiempo interno del modelo.
- `delta_ext` cambia el estado cuando llega un evento externo.
- `lambda` genera la salida antes de ejecutar la transicion interna.
- `ta` indica cuanto tiempo falta para el proximo evento interno.

En PowerDEVS esto se implementa con los metodos habituales:

```cpp
void init(double t,...);
double ta(double t);
void dext(Event e, double t);
Event lambda(double t);
void dint(double t);
void exit();
```

La variable que en esta libreria suele llamarse `sigma` representa el valor que
devuelve `ta`. Si `sigma = INFINITY`, el modelo queda pasivo hasta que llegue un
evento externo. Si `sigma = 0`, el modelo queda listo para producir una salida
inmediata.

La secuencia usual ante una entrada es:

```text
dext recibe Event
dext actualiza estado interno
dext pone sigma = processing_time o sigma = 0
cuando vence sigma, PowerDEVS llama lambda
PowerDEVS entrega el Event de salida por el puerto indicado
PowerDEVS llama dint
dint limpia el estado temporal y vuelve a sigma = INFINITY
```

Un modelo acoplado DEVS contiene modelos atomicos o acoplados y define
conexiones entre puertos. No deberia esconder dinamica propia fuera de sus
submodelos. En esta libreria `Quantum Circuit` se usa como contenedor acoplado:
la dinamica vive en atomicos, pero todos comparten un contexto asociado al
acoplado.

## Mapeo cuantico a DEVS

Un circuito cuantico tiene un estado global. No es correcto que cada cable tenga
un estado independiente, porque la superposicion y el entrelazamiento pertenecen
al sistema completo.

Por eso la libreria usa dos niveles:

- `QuantumContext`: estado cuantico global compartido por un `Quantum Circuit`.
- `QuantumWire`: referencia liviana que viaja por los puertos DEVS.

`QuantumWire` contiene:

```cpp
struct QuantumWire {
    QuantumContext* context;
    int qubit;
};
```

El cable no transporta amplitudes. Transporta:

- a que contexto pertenece;
- que indice logico de qubit representa.

Entonces una compuerta de un qubit hace:

```text
recibir QuantumWire
leer wire.context
leer wire.qubit
aplicar matriz al StateVector global del contexto
reenviar el mismo cable
```

Una compuerta de dos qubits hace:

```text
recibir QuantumWire por puerto 0
recibir QuantumWire por puerto 1
verificar que ambos apuntan al mismo QuantumContext
aplicar la operacion al StateVector global
reenviar ambos cables por sus salidas
```

`Measure` es la frontera cuantico-clasica:

```text
recibe QuantumWire
calcula probabilidades desde el StateVector global
elige 0 o 1 usando RNG
colapsa el StateVector
emite double 0 por puerto 0 o double 1 por puerto 1
```

## Archivos principales

La implementacion cuantica vive en:

```text
atomics/quantum/
```

Archivos centrales:

- `quantum_context.h/.cpp`: contexto compartido, estado global, shots,
  asignacion de qubits, medicion y limpieza.
- `qstate.h`: estructura `QuantumState` con `N`, `dim`, `psi` y mediciones.
- `quantum_common.h`: parseo simple de parametros y helpers `apply1`, `apply2`.
- `quantum_core.h/.cpp`: configura el contexto desde la UI.
- `qzero.h/.cpp`: fuente de qubit inicial `|0>`.
- `qone.h/.cpp`: fuente de qubit inicial `|1>`.
- `h`, `x`, `y`, `z`, `id`: compuertas de un qubit.
- `cx.h/.cpp`: `CNOT`.
- `measure.h/.cpp`: medicion proyectiva.
- `qcount.h/.cpp`: contador clasico especifico para resultados 0/1.

La descripcion visual de la libreria esta en:

```text
library/quantum/quantum.pdl
library/quantum/*.svg
```

Los ejemplos actuales estan en:

```text
examples/Quantum/
```

El contador general usado para histogramas simples esta en:

```text
atomics/sinks/ev_count2.cpp
library/sinks/sinks.pdl
```

## Quantum Circuit

`Quantum Circuit` es un modelo acoplado de la libreria. Su funcion es dar un
espacio visual donde se conectan fuentes, compuertas y mediciones.

Decision importante: `Quantum Circuit` no tiene que tener puertos especiales ni
codigo propio para mover amplitudes. En DEVS, el comportamiento se expresa con
modelos atomicos y eventos. El acoplado organiza el circuito, y el estado
compartido se encuentra por el padre comun de los atomicos.

La clave del contexto se calcula con el nombre completo del padre:

```cpp
static std::string context_key_for(Simulator* model) {
    if (!model || !model->father) return "__quantum_root__";
    return model->father->getFullName();
}
```

Asi, todos los atomicos colocados dentro del mismo `Quantum Circuit` apuntan al
mismo `QuantumContext`.

## Quantum Core

`Quantum Core` es un modelo atomico sin entradas ni salidas. Parece vacio en la
UI, pero cumple una funcion central: configura el contexto compartido.

Parametros:

```text
Number of qubits
Shots
Shot period
```

En `init`, `Quantum Core` hace:

```text
ctx = quantum_context_for(this, nqubits)
ctx->configure(nqubits, "0")
ctx->configure_shots(shots, period)
```

En `ta` queda pasivo:

```text
sigma = INFINITY
```

En `exit` limpia el contexto:

```text
clear_quantum_context_for(this)
```

Decision: `Quantum Core` no tiene puertos porque no representa una compuerta ni
un cable. Es configuracion global del circuito. Si tuviera puertos, el usuario
tendria que conectarlo manualmente y se ensuciaria la UI sin agregar dinamica
DEVS util.

## QuantumContext

`QuantumContext` guarda:

- cantidad de qubits (`nqubits`);
- dimension del vector (`2^nqubits`);
- amplitudes complejas (`psi_storage`);
- mediciones registradas (`meas_storage`);
- bits iniciales (`initial_bits`);
- cantidad de shots;
- periodo entre shots;
- indice automatico siguiente;
- bandera `locked`.

La estructura exportada para operaciones de bajo nivel es:

```cpp
struct QuantumState {
    int N;
    int dim;
    std::complex<double>* psi;
    int* measurements;
};
```

El estado base se reconstruye con `rebuild_basis_state`. Si los bits iniciales
son `q0 = 0` y `q1 = 1`, el indice base se arma con:

```text
idx |= (1 << q)
```

Esta convencion usa `q0` como bit menos significativo.

### Shots

Un shot es una repeticion del mismo circuito desde el mismo estado inicial. La
libreria lo modela como eventos dentro de una misma simulacion DEVS.

Las fuentes (`Q Zero`, `Q One`) emiten en:

```text
t = 0
t = Shot period
t = 2 * Shot period
...
```

Antes de emitir cada shot, llaman:

```cpp
wire.context->begin_shot(shot);
```

`begin_shot` reconstruye el estado base. Por eso no hace falta correr muchas
simulaciones completas para estimar un histograma de mediciones: cada shot es un
evento nuevo dentro de la misma simulacion.

## quantum_context_for

`quantum_context_for` es la funcion que une los atomicos de un circuito.

Hace tres cosas:

1. Calcula una clave usando el padre del modelo (`model->father->getFullName()`).
2. Busca o crea un `QuantumContext` en un `std::map` global.
3. Si el contexto todavia no esta configurado, lo inicializa con al menos
   `min_qubits`.

Codigo conceptual:

```text
key = nombre completo del acoplado padre
ctx = quantum_contexts[key]
si ctx no existe/configurado, configure(min_qubits, "0")
devolver &ctx
```

Esto permite que `H`, `CNOT`, `Measure`, `Q Zero` y `Q One` no tengan que estar
conectados al `Quantum Core`. Todos encuentran el mismo contexto por estar
dentro del mismo `Quantum Circuit`.

Limitacion: el mapa global no es una solucion thread-safe. Para el uso actual de
PowerDEVS en una simulacion local esta bien, pero si se ejecutaran multiples
simulaciones paralelas habria que revisar el aislamiento de contextos.

## Q Zero y Q One

`Q Zero` y `Q One` son fuentes DEVS. No reciben entradas y emiten un
`QuantumWire`.

Parametro:

```text
Qubit index
```

Valores:

- `-1`: asignacion automatica.
- `0` o mayor: indice manual.

En `init`:

```text
leer parametro
buscar contexto
si indice < 0, pedir allocate_qubit()
registrar bit inicial 0 o 1
programar primera salida con sigma = 0
```

En `lambda`:

```text
begin_shot(shot)
wire.context = contexto
wire.qubit = indice
emitir wire por puerto 0
```

En `dint`:

```text
shot++
si quedan shots, sigma = Shot period
si no quedan shots, sigma = INFINITY
```

### Por que existe `Qubit index`

El indice existe porque el estado global necesita saber que posicion del
`StateVector` corresponde a cada fuente. En un circuito de dos qubits, no alcanza
con ver dos cables en pantalla: internamente hay que saber cual es `q0` y cual
es `q1`.

Para evitar configurar todo a mano se agrego `-1`, que asigna automaticamente.
Esto es comodo para ejemplos simples.

Limitacion: la asignacion automatica depende del orden de inicializacion de los
bloques en el modelo. Si un circuito grande necesita reproducibilidad fuerte, es
mejor usar indices manuales.

## Compuertas de un qubit

Las compuertas `H`, `X`, `Y`, `Z` e `ID` tienen:

```text
Ports = 1 ; 1
Processing time
```

Reciben un `QuantumWire`, aplican una matriz de 2x2 al qubit indicado y
reemiten el mismo cable.

Ejemplo conceptual para `H`:

```text
dext:
  wire = entrada
  U = Hadamard
  wire.context->apply_one(wire.qubit, U)
  sigma = processing_time

lambda:
  emitir wire por puerto 0

dint:
  sigma = INFINITY
```

`apply_one` recorre el `StateVector` de a pares de amplitudes. Para el qubit `q`,
usa la mascara:

```text
m = 1 << q
```

Luego toma los indices que solo difieren en ese bit y aplica la matriz.

## CNOT

`CNOT` tiene:

```text
Ports = 2 ; 2
Processing time
```

Convencion:

```text
entrada 0 = control
entrada 1 = target
salida 0 = control
salida 1 = target
```

`CNOT` no puede aplicar la operacion al recibir solo un cable. Por eso guarda
dos banderas:

```text
has_control
has_target
```

Cuando llegan ambos cables:

```text
verifica que ambos tengan contexto
verifica que ambos pertenezcan al mismo QuantumContext
aplica CNOT
programa salida por puerto 0
```

Como un `lambda` solo devuelve un `Event`, `CNOT` emite las dos salidas en dos
eventos internos consecutivos:

```text
lambda puerto 0
dint pone emit_port = 1 y sigma = 0
lambda puerto 1
dint limpia banderas y queda pasivo
```

Esta decision mantiene el contrato DEVS simple y evita inventar un evento
multiple.

## Measure

`Measure` tiene:

```text
Ports = 1 ; 2
Processing time
Random seed
```

La salida clasica se codifica de dos maneras al mismo tiempo:

- valor `double`: `0.0` o `1.0`;
- puerto: `0` si salio cero, `1` si salio uno.

Esto permite dos usos:

1. Conectar a `Q Count`, que lee el valor `double`.
2. Conectar los puertos `0` y `1` a `Event Counter`, que cuenta eventos por
   puerto y genera histograma.

La medicion calcula:

```text
p0 = suma de |psi[i]|^2 donde el bit q de i es 0
outcome = 0 con probabilidad p0, 1 con probabilidad 1 - p0
```

Luego colapsa:

```text
amplitudes incompatibles con outcome = 0
amplitudes compatibles /= sqrt(probabilidad del outcome)
```

`Random seed = 0` usa `std::random_device`, o sea resultados no deterministas
entre corridas. Un seed mayor que cero permite reproducir la secuencia.

## Q Count

`Q Count` es un contador clasico especifico para mediciones cuanticas.

Tiene:

```text
Ports = 1 ; 2
```

Recibe un `double`:

```text
valor < 0.5 -> cuenta ceros
valor >= 0.5 -> cuenta unos
```

Emite:

```text
salida 0 = cantidad acumulada de ceros
salida 1 = cantidad acumulada de unos
```

Como `CNOT`, emite dos valores con eventos internos consecutivos. Es util para
graficar curvas acumuladas en tiempo con `GnuPlot`.

Para un histograma final simple, hoy se recomienda `Measure -> Event Counter`.

## Event Counter para histogramas

`Event Counter` es un sink general de PowerDEVS que cuenta eventos por puerto.
Fue adaptado para funcionar bien con parametros de la UI y para producir un
histograma final.

Uso con `Measure`:

```text
Measure salida 0 -> Event Counter entrada 0
Measure salida 1 -> Event Counter entrada 1
Event Counter Ports = 2 ; 2
Event Counter Inputs = %Inputs
Event Counter File = hadamard_counts.txt
```

Como `Measure` emite por el puerto igual al resultado, el contador termina con:

```text
k[0] = cantidad de veces que salio 0
k[1] = cantidad de veces que salio 1
```

En `exit`, `Event Counter` escribe:

```text
hadamard_counts.txt
hadamard_counts.txt.hist.dat
hadamard_counts.txt.hist.plt
```

El `.txt` contiene un valor por linea. El `.hist.dat` tiene pares:

```text
0 count0
1 count1
```

El `.hist.plt` abre gnuplot con barras de colores separados:

```text
result 0 -> azul
result 1 -> rojo
```

Decision: no se uso el `GnuPlot` clasico para el histograma final porque ese
bloque esta pensado para graficar series temporales. El histograma es un resumen
de fin de simulacion.

## Circuito minimo: Hadamard + Measure + histograma

Archivo de ejemplo:

```text
examples/Quantum/hadamard_count_simple.pdm
```

Circuito:

```text
Q Zero -> H -> Measure
Measure salida 0 -> Event Counter entrada 0
Measure salida 1 -> Event Counter entrada 1
```

Ademas, dentro del mismo `Quantum Circuit` debe estar:

```text
Quantum Core
```

Parametros recomendados:

```text
Quantum Core:
  Number of qubits = 1
  Shots = 100
  Shot period = 1

Q Zero:
  Qubit index = -1

H:
  Processing time = 0.1

Measure:
  Processing time = 0.1
  Random seed = 0

Event Counter:
  Ports = 2 ; 2
  Inputs = %Inputs
  File = hadamard_counts.txt
```

Para `Shots = 100` y `Shot period = 1`, usar:

```text
FinalTime = 105
```

Si `FinalTime = 10`, solo entran aproximadamente diez shots. En ese caso el
histograma puede mostrar, por ejemplo, `4` y `6`, que no es un error del modelo:
la simulacion termino antes de completar los 100 shots configurados.

Resultado esperado:

```text
H |0> = (|0> + |1>) / sqrt(2)
```

Entonces el histograma deberia tender a:

```text
0: 50%
1: 50%
```

No tiene que dar exactamente 50 y 50 en cada corrida, porque la medicion es
aleatoria.

## Circuito Bell anticorrelacionado

Un ejemplo de dos qubits:

```text
Q Zero -> H -> CNOT control
Q One  --------> CNOT target
CNOT salida 0 -> Measure q0
CNOT salida 1 -> Measure q1
```

Si el estado inicial es `|01>` y se aplica `H` al qubit 0 seguido de `CNOT`, el
estado ideal antes de medir es:

```text
(|01> + |10>) / sqrt(2)
```

Por eso los resultados son anticorrelacionados:

```text
si q0 = 0, q1 = 1
si q0 = 1, q1 = 0
```

Para contar ambos qubits se puede usar:

- un `Event Counter` por medicion;
- o `Q Count` por medicion y un `GnuPlot` con curvas acumuladas.

## GnuPlot para curvas acumuladas

Si se quiere ver la evolucion temporal de conteos, usar `Q Count` y `GnuPlot`.

Para cuatro entradas:

```text
Inputs = %Inputs
Format = set xrange [0:%tf] @ set yrange [0:20] @ set ytics 0,2,20 @ set grid ytics @ set xlabel 'simulation time' @ set ylabel 'accumulated counts' @ set title 'Measurement counts after 20 shots'
Format1 = with steps lw 2 lc rgb '#1f77b4' title 'q0 -> 0'
Format2 = with steps lw 2 lc rgb '#d62728' title 'q0 -> 1'
Format3 = with steps lw 2 dt 2 lc rgb '#2ca02c' title 'q1 -> 0'
Format4 = with steps lw 2 dt 2 lc rgb '#9467bd' title 'q1 -> 1'
```

Si se cambia `Shots`, ajustar `yrange` y `ytics`.

## Multiples corridas vs shots

PowerDEVS tiene ejemplos de multiples simulaciones completas usando mecanismos
como:

```text
SimulationExperimentTracker
RunScilabJob
multipleSimulationCommands
```

Ese patron sirve para barrer parametros entre simulaciones, por ejemplo:

- comparar varios circuitos;
- variar una constante fisica;
- cambiar una seed por corrida;
- guardar resultados de cada corrida en archivos separados.

Para un experimento cuantico repetido, lo correcto y mas simple es usar `Shots`
dentro de la misma simulacion:

```text
cada shot reinicia el StateVector
cada shot recorre el mismo circuito
cada Measure produce un resultado clasico
un contador acumula frecuencias
```

Esto representa el experimento cuantico sin pagar el costo ni la complejidad de
lanzar PowerDEVS muchas veces.

## Decisiones de diseno

### No pasar StateVector por cables

Pasar un `StateVector` por cada cable parece directo, pero rompe el significado
fisico cuando hay entrelazamiento. El estado pertenece al sistema completo, no a
un cable individual.

Por eso el cable solo lleva una referencia (`QuantumWire`) y las compuertas
mutan el contexto compartido.

### Usar Quantum Core sin puertos

`Quantum Core` configura el contexto. No representa una transformacion del
circuito. Si se obligara al usuario a conectarlo, se mezclaria configuracion con
flujo de eventos y la UI seria menos clara.

### Contexto por modelo acoplado padre

Usar `model->father->getFullName()` permite que todos los atomicos dentro de un
`Quantum Circuit` encuentren el mismo contexto sin cablear dependencias
auxiliares.

### Auto indice con `-1`

Se agrego para que circuitos simples no pidan configuracion extra. Sigue
existiendo el indice manual porque los circuitos grandes necesitan control
explicito sobre que cable representa cada qubit.

### Measure emite por puerto resultado

Al principio alcanzaba con emitir el valor `0/1`. Para contar con bloques DEVS
existentes conviene ademas codificar el resultado en el puerto. Asi se puede
usar `Event Counter` sin crear un contador cuantico nuevo para cada caso.

### Histograma en Event Counter

El histograma final es un resumen de frecuencias, no una serie temporal.
Generarlo en `exit` del contador hace que el grafico represente el resultado del
experimento completo.

### Parseo simple de parametros

Algunos atomicos antiguos usaban infraestructura de `ParameterReader` que
arrastraba dependencias de Scilab o simbolos extra. Para parametros simples que
la UI pasa como strings, la libreria usa `std::atof` o `std::atoi`.

## Problemas encontrados

### Undefined reference to vtable

Sintoma:

```text
undefined reference to vtable for ...
```

Causa probable: el header del atomic fue usado por el modelo generado, pero el
`.cpp` correspondiente no estaba linkeado dentro de `libmodels.so`.

Solucion:

```text
compilar el .cpp
relinkear libmodels.so
```

### Error ABI en getFullName

Sintoma:

```text
Simulator::getFullName[abi:cxx11]
```

Causa: objeto compilado con ABI C++11 nueva contra PowerDEVS compilado con ABI
vieja.

Solucion usada:

```text
-D_GLIBCXX_USE_CXX11_ABI=0
```

### Versiones GLIBC/GLIBCXX/CXXABI

Sintoma:

```text
GLIBC_2.29 not found
GLIBCXX_3.4.22 not found
CXXABI_1.3.8 not found
```

Causa: `libmodels.so` arrastro objetos o dependencias compiladas con una
toolchain mas nueva que la esperada por el entorno donde corre PowerDEVS.

Solucion practica usada durante la integracion:

```text
linkear un libmodels.so minimo con atomics/quantum/*.o, gnuplot.o y ev_count2.o
```

Esto fue suficiente para correr los ejemplos cuanticos. Si se necesita toda la
libreria de atomicos, hay que reconstruir el conjunto completo con una toolchain
compatible.

### Dependencias de Scilab en atomicos simples

Sintomas:

```text
Scilab2CmdLineParameterReader
getReadingBackend
printLog(int, ...)
```

Causa: se arrastraban dependencias por usar lectores de parametros complejos
para convertir valores simples.

Solucion: parsear parametros simples directamente desde `char*`.

### Todo colapsa en t = 0

Si todos los `Processing time` son cero, la cadena completa puede ejecutarse en
el mismo instante de simulacion. Eso no significa que el resultado sea
fisicamente incorrecto, pero hace que las curvas se vean superpuestas o que no
se entienda la secuencia.

Recomendacion:

```text
Processing time = 0.1
Shot period = 1
FinalTime > Shots * Shot period
```

### Conteos mas bajos que Shots

Si se configuran 100 shots pero el histograma suma 10, revisar `FinalTime`.

Regla practica:

```text
FinalTime >= Shots * Shot period + margen
```

Ejemplo:

```text
Shots = 100
Shot period = 1
FinalTime = 105
```

### No aparece gnuplot

Revisar:

- que exista `/usr/bin/gnuplot`;
- que el directorio de salida pueda escribirse;
- que la simulacion haya llegado a `exit`;
- que el archivo configurado en `Event Counter` sea valido.

## Como extender la libreria

### Agregar una compuerta de un qubit

Pasos:

1. Crear `atomics/quantum/nueva.h` y `nueva.cpp`.
2. Copiar el patron de `h.cpp`, `x.cpp`, `z.cpp` o `id.cpp`.
3. Leer `Processing time` con `qparse_d`.
4. En `dext`, castear `Event.value` a `QuantumWire*`.
5. Definir la matriz `U[4]`.
6. Llamar `wire.context->apply_one(wire.qubit, U)`.
7. En `lambda`, emitir el `QuantumWire`.
8. Agregar el atomic a `library/quantum/quantum.pdl`.
9. Agregar un icono SVG en `library/quantum/`.
10. Compilar el `.cpp` y relinkear `libmodels.so`.

### Agregar una compuerta de dos qubits

Pasos:

1. Usar el patron de `cx.cpp`.
2. Guardar un `QuantumWire` por puerto.
3. Esperar a tener ambos cables.
4. Verificar que ambos contextos sean iguales.
5. Aplicar la operacion al `QuantumContext`.
6. Emitir las salidas en eventos internos consecutivos si hacen falta dos
   eventos de salida.

### Agregar un bloque clasico

Si el bloque procesa resultados de medicion, deberia recibir `double`, no
`QuantumWire`.

Ejemplos:

- contador;
- normalizador de frecuencias;
- exportador CSV;
- acumulador por bitstring.

Si el bloque necesita distinguir `0` y `1`, puede usar:

- el valor (`double`);
- el puerto de entrada;
- o ambos.

### Agregar soporte para bitstrings

El esquema actual de `Measure` es comodo para un qubit medido por bloque. Para
histogramas de bitstrings completos, convendria agregar un bloque clasico que
reciba varias mediciones de un mismo shot y construya claves tipo:

```text
00
01
10
11
```

Ese bloque deberia conocer:

- cuantos qubits espera;
- a que shot pertenece cada medicion;
- cuando cerrar un bitstring.

No conviene meter esa responsabilidad en `Measure`, porque `Measure` representa
una medicion local.

## Checklist de prueba

Para un cambio en atomicos cuanticos:

```text
compilar el .cpp modificado
relinkear libmodels.so
verificar que PowerDEVS cargue el modelo
correr examples/Quantum/hadamard_count_simple.pdm
verificar que el histograma suma Shots
correr un circuito de dos qubits si se toco CNOT o contexto
revisar que no aparezcan errores ABI
```

Para un cambio visual:

```text
abrir la libreria en la UI
ver que el icono aparezca
crear un bloque nuevo
confirmar puertos y parametros
guardar y ejecutar un ejemplo minimo
```

## Limitaciones actuales

- `QuantumContext` usa un `std::map` global y no esta preparado para ejecucion
  paralela sin revisar sincronizacion.
- La asignacion automatica de qubits depende del orden de inicializacion.
- La memoria crece como `2^N`; es simulacion state-vector, no tensor network ni
  estabilizadores.
- `Event Counter` mantiene un arreglo fijo `k[20]`; no usar mas de 20 puertos
  sin modificarlo.
- El `libmodels.so` usado durante la integracion puede ser minimo; si se quieren
  todos los atomics historicos de PowerDEVS, hay que reconstruirlos con ABI y
  toolchain compatibles.
- `Measure` colapsa inmediatamente el estado global; el orden de mediciones
  simultaneas puede importar si se construyen modelos mas complejos.
- Los modelos viejos que asumian `Measure` con una sola salida deben actualizar
  conexiones si quieren aprovechar el conteo por puerto.

## Resumen operativo

Para un circuito Hadamard con histograma:

```text
Quantum Circuit
  Quantum Core: Number of qubits = 1, Shots = 100, Shot period = 1
  Q Zero: Qubit index = -1
  H: Processing time = 0.1
  Measure: Processing time = 0.1, Random seed = 0
  Event Counter: Ports = 2 ; 2, Inputs = %Inputs, File = hadamard_counts.txt

Conexiones:
  Q Zero -> H
  H -> Measure
  Measure salida 0 -> Event Counter entrada 0
  Measure salida 1 -> Event Counter entrada 1

FinalTime:
  105 para 100 shots con periodo 1
```

El resultado esperado no es una linea temporal sino un histograma final con dos
barras:

```text
resultado 0
resultado 1
```

Para `H |0>`, ambas barras deberian estar cerca del 50 por ciento con suficiente
cantidad de shots.
