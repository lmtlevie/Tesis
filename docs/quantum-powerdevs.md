# Quantum PowerDEVS notes

Este documento resume la arquitectura actual de la libreria cuantica, como armar
un circuito desde la UI, como graficar conteos de medicion y que errores
aprendimos a diagnosticar durante la integracion.

## Idea central

Las compuertas no transportan un `StateVector` completo por cada cable. Cada
cable transporta un `QuantumWire`, que contiene:

- un puntero al `QuantumContext` compartido del `Quantum Circuit`;
- el indice logico del qubit dentro de ese contexto.

El `QuantumContext` vive asociado al modelo acoplado `Quantum Circuit` y guarda:

- el `StateVector` global de todos los qubits;
- los bits iniciales declarados por `Q Zero` y `Q One`;
- la asignacion automatica de indices;
- la configuracion de shots para repetir experimentos.

Esto evita la contradiccion de tener un estado independiente por cable. Una
compuerta como `H` recibe un `QuantumWire`, mira `wire.qubit`, y aplica su matriz
al `StateVector` compartido. `CNOT` recibe dos `QuantumWire`: puerto 0 control,
puerto 1 target. `Measure` colapsa el estado compartido y emite un valor clasico
`double`: `0` o `1`.

## Bloques actuales

- `Quantum Circuit`: modelo acoplado donde se arma el circuito.
- `Quantum Core`: estado compartido y configuracion global.
- `Q Zero`: inicializa un qubit en `|0>`.
- `Q One`: inicializa un qubit en `|1>`.
- `H`, `X`, `Y`, `Z`, `ID`: compuertas de un qubit.
- `CNOT`: compuerta de dos qubits; entrada 0 control, entrada 1 target.
- `Measure`: medicion proyectiva; salida clasica `0` o `1`.
- `Q Count`: contador clasico; entrada `0/1`, salida 0 = cantidad de ceros,
  salida 1 = cantidad de unos.
- `GnuPlot`: grafica senales clasicas.

## Parametros importantes

`Quantum Core`:

- `Number of qubits`: cantidad total de qubits del circuito.
- `Shots`: cantidad de repeticiones del experimento dentro de una simulacion.
- `Shot period`: separacion temporal entre el inicio de un shot y el siguiente.

`Q Zero` / `Q One`:

- `Qubit index = -1`: asignacion automatica.
- `Qubit index >= 0`: asignacion manual.

La asignacion automatica depende del orden de inicializacion de los bloques en
el modelo. Para circuitos simples conviene dejar `-1`; para circuitos grandes o
cuando el orden sea ambiguo, usar indices explicitos.

Compuertas y `Measure`:

- `Processing time`: retardo DEVS del bloque.

No conviene dejar todo en `0` cuando se quiere mirar el grafico. Si todos los
tiempos son cero, todo ocurre en `t=0` y la visualizacion queda superpuesta. Para
depurar, usar tiempos pequenos como `0.1`, `0.25` o `0.5`.

## Circuito ejemplo: Bell anticorrelacionado

En `examples/Quantum/prueba1.pdm` el circuito es:

1. `Q Zero` emite el qubit 0 en `|0>`.
2. `Q One` emite el qubit 1 en `|1>`.
3. `H` se aplica al qubit 0.
4. `CNOT` usa qubit 0 como control y qubit 1 como target.
5. Dos `Measure` miden ambos qubits.
6. Dos `Q Count` acumulan cuantos `0` y `1` salieron para cada qubit.
7. `GnuPlot` grafica los cuatro conteos acumulados.

El estado ideal antes de medir es:

```text
(|01> + |10>) / sqrt(2)
```

Por eso los resultados esperados son anticorrelacionados: cuando un qubit mide
`0`, el otro mide `1`, y viceversa.

## Como armarlo en la UI

Dentro de `Quantum Circuit`:

1. Poner un solo `Quantum Core`.
2. Configurar `Quantum Core`:

```text
Number of qubits = 2
Shots = 20
Shot period = 2
```

3. Poner `Q Zero` y `Q One`, ambos con:

```text
Qubit index = -1
```

4. Conectar `Q Zero -> H -> CNOT puerto 0`.
5. Conectar `Q One -> CNOT puerto 1`.
6. Conectar las dos salidas de `CNOT` a dos `Measure`.
7. Conectar cada `Measure` a un `Q Count`.
8. Conectar cada `Q Count` al `GnuPlot`:

```text
Q Count q0 salida 0 -> GnuPlot entrada 1
Q Count q0 salida 1 -> GnuPlot entrada 2
Q Count q1 salida 0 -> GnuPlot entrada 3
Q Count q1 salida 1 -> GnuPlot entrada 4
```

9. Usar `FinalTime` mayor que `Shots * Shot period`. Para 20 shots con periodo
   2, usar por ejemplo:

```text
FinalTime = 42
```

## GnuPlot para conteos

Para cuatro entradas:

```text
Inputs = %Inputs
Format = set xrange [0:%tf] @ set yrange [0:20] @ set ytics 0,2,20 @ set grid ytics @ set xlabel 'simulation time' @ set ylabel 'accumulated counts' @ set title 'Measurement counts after 20 shots'
Format1 = with steps lw 2 lc rgb '#1f77b4' title 'q0 -> 0'
Format2 = with steps lw 2 lc rgb '#d62728' title 'q0 -> 1'
Format3 = with steps lw 2 dt 2 lc rgb '#2ca02c' title 'q1 -> 0'
Format4 = with steps lw 2 dt 2 lc rgb '#9467bd' title 'q1 -> 1'
```

Si se cambia `Shots`, ajustar `set yrange [0:20]` y `set ytics 0,2,20` al nuevo
maximo esperado.

## Como fluyen los eventos

Cada shot empieza cuando `Q Zero` y `Q One` emiten sus `QuantumWire`. Al comenzar
un nuevo shot, el `QuantumContext` reconstruye el estado base con los bits
iniciales definidos por las fuentes.

Luego:

- las compuertas reciben `QuantumWire`;
- modifican el `StateVector` compartido;
- reemiten el mismo `QuantumWire` para seguir el cable logico;
- `Measure` convierte el resultado cuantico en un evento clasico `double`;
- `Q Count` acumula esos eventos clasicos;
- `GnuPlot` solo ve numeros clasicos.

## Shots vs multiples simulaciones

En PowerDEVS existen ejemplos de "multiples simulation runs" usando:

- `SimulationExperimentTracker`
- `RunScilabJob`
- `multipleSimulationCommands`

Ese patron aparece en modelos como `examples/dde/oberle_and_pesch.pdm` y sirve
para barrer parametros entre corridas completas, guardar resultados en Scilab y
ejecutar comandos al final de cada simulacion.

Para estimar resultados de una medicion cuantica no hace falta lanzar muchas
simulaciones completas. Un "shot" cuantico es repetir el mismo circuito con el
mismo estado inicial y volver a medir. En DEVS esto se representa naturalmente
como eventos dentro de una misma simulacion:

```text
Q Zero / Q One reemiten cada Shot period
QuantumContext reinicia el StateVector al comienzo de cada shot
Measure produce 0 o 1
Q Count acumula frecuencias
```

Tambien existen contadores generales en `sinks`, por ejemplo `Event Counter`,
pero cuentan cantidad de eventos por puerto y guardan a archivo. Para mediciones
cuanticas necesitamos contar por valor (`0` o `1`) y graficar durante la
simulacion; por eso `Q Count` es un contador especifico para la salida clasica de
`Measure`.

Usar `SimulationExperimentTracker` tendria sentido si se quiere comparar muchas
configuraciones distintas, por ejemplo:

- distintos circuitos;
- distintos valores de parametros;
- distintos numeros de qubits;
- distintos seeds controlados por corrida.

Para un histograma de un mismo circuito, usar `Shots` + `Q Count`.

## Errores que aprendimos a reconocer

`undefined reference to vtable for ...`

Significa que el header se incluyo en el modelo generado, pero el `.cpp` del
atomic no esta dentro de `libmodels.so`. Solucion: recompilar el atomic y relinkear
`libmodels.so`.

`Simulator::getFullName[abi:cxx11]`

El objeto nuevo fue compilado con ABI C++11 nueva, pero PowerDEVS estaba usando
la ABI vieja. Solucion usada:

```text
-D_GLIBCXX_USE_CXX11_ABI=0
```

`GLIBC_2.29`, `GLIBCXX_3.4.22`, `CXXABI_1.3.8`

El `libmodels.so` fue relinkeado arrastrando objetos compilados con una toolchain
mas nueva que la esperada por el entorno de PowerDEVS. Para el ejemplo cuantico
usamos un `libmodels.so` minimo con solo los atomics necesarios:

```text
atomics/quantum/*.o
atomics/sinks/gnuplot.o
```

`Scilab2CmdLineParameterReader`, `getReadingBackend`, `printLog(int, ...)`

`GnuPlot` dependia de `ParameterReader` solo para convertir el parametro
`Inputs`. Eso arrastraba demasiada infraestructura. Se cambio `gnuplot.cpp` para
parsear ese numero directamente con `atof`, porque la UI ya pasa un string
literal como `"4.000000e+00"`.

Todo aparece en `t=0`

No es necesariamente que el resultado sea cero. Si todos los `Processing time`
son `0`, DEVS ejecuta toda la cadena en el mismo instante. Para visualizar, poner
tiempos positivos y usar shots separados por `Shot period`.

Resultados siempre iguales

Revisar:

- que `Measure` tenga `Random seed = 0` si se quiere aleatoriedad real;
- que `Shots` sea mayor que 1;
- que `FinalTime` alcance a cubrir todos los shots;
- que el circuito realmente genere superposicion antes de medir.

No aparece ventana de gnuplot o falla al crear archivos

`gnuplot` usa el directorio `plots`. Si no existe, puede fallar la creacion de
archivos. Tambien verificar que `/usr/bin/gnuplot` exista.

## Regla practica para extender la libreria

- Si el bloque representa una compuerta cuantica, debe recibir y emitir
  `QuantumWire`.
- Si el bloque mide o procesa resultados clasicos, debe emitir `double`.
- El unico dueno del `StateVector` debe ser `QuantumContext`.
- `Quantum Circuit` organiza el modelo, pero la memoria compartida vive en un
  atomic/contexto asociado al acoplado.
- Evitar depender de `ParameterReader` dentro de atomics nuevos si la UI ya pasa
  parametros literales simples.
