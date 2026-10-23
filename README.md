# OAC Karel Docker

Este repositorio contiene la simulacion ROS Melodic de Karel, un robot diferencial basico con una huella de
12 x 14 cm, dos ruedas motrices y cuatro sensores ultrasonicos.

Además, contiene el emulador del procesador M68HC11, específicamente para evaluar
el conocimiento adquirido en la clase de Organización y Arquitectura de Computadoras.

## Requisitos

- Docker Engine con Docker Compose en Linux, o Docker Desktop en Windows y
  macOS.
- Al menos 4 CPU, 8 GB de RAM asignados a Docker y 12 GB libres en disco.
- En Apple Silicon, Docker Desktop debe permitir emulacion `linux/amd64`; el
  workspace heredado aun no dispone de una imagen ARM64.

## Instalación de Docker

En el caso de Linux sigue los pasos en `https://docs.docker.com/engine/install/` de acuerdo
a la distribución que usas.

En el caso de Windows y macOS, por favor instala **Docker Desktop**.

### Instalación rápida en Windows

Abre una terminal de powershell y verifíca la instalación de **WSL**:

```bash
wsl --version
```

Si no existe, debes instalarlo:

```bash
wsl --install
```

Y finalmente actualizar:

```bash
wsl --update
```

Visita `https://docs.docker.com/desktop/setup/install/windows-install/` y descarga el
instalador correspondiente.

Haz doble clic en **Docker Desktop Installer.exe** para ejecutar el instalador.
El instalador te preguntará qué modo de instalación prefieres. La recomendación es
elegir "Por usuario" para que Windows no pida privilegios de administrador al ejecutar la aplicación.

Docker Desktop no se inicia automáticamente después de la instalación. Para iniciar Docker Desktop:
Busca Docker y selecciona Docker Desktop en los resultados de la búsqueda.
Ten en cuenta que Docker Desktop no se ejecutará si no acepta los términos.

La primera vez que ejecutes la aplicación te pedira que ejecutes el siguiente comando
en powershell:

```bash
- Enable-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform
```

Debes esperar un momento a que se cargue la configuración.
Para ocupar los comandos de docker y docker compose lo debes hacer desde una terminal
en powershell.

Para más información consulta `https://docs.docker.com/desktop/setup/install/windows-install/#wsl-verification-and-setup`
y `https://docs.docker.com/desktop/setup/install/windows-install/`

## Inicio

Construir y arrancar la primera vez o despues de cambiar dependencias:

```sh
docker compose build simulator workspace
docker compose up -d
```

La primera construccion descarga varios GB y puede tardar. Los siguientes
arranques reutilizan la cache de Docker. Jupyter genera un token aleatorio si
`JUPYTER_TOKEN` no esta definido. Para usar uno estable, copie `.env.example`
como `.env` y cambie su valor.

Para equipos con pocos recursos se puede desactivar solamente la ventana de
Gazebo con `KAREL_GUI=false` en `.env`; ROS, la evaluacion y Jupyter continuan
funcionando. RViz puede iniciarse despues desde el workspace.

El codigo ROS vive en `src/` y no se incluye en la imagen del simulador. El
contenedor lo monta en modo lectura, crea una copia interna y compila los
paquetes de simulacion al arrancar. Despues de cambiar codigo, launch files,
mundos o configuracion solamente es necesario reiniciar:

```sh
docker compose restart simulator
```

No es necesario reconstruir la imagen salvo que cambien los Dockerfiles o las
dependencias APT/ROS.

## World

El mundo se elige solamente al arrancar. Los nombres disponibles se consultan
con:

```sh
docker compose exec simulator bash -lc 'source /catkin_ws/devel/setup.bash && rosrun karel_gazebo list_worlds.py'
```

`circuit_single_obstacle` es la pista predeterminada de 2.0 x 0.5 m. Contiene
un unico obstaculo estatico cerca del centro, siempre en la misma posicion. Para
arrancarla o volver a ella despues de seleccionar otro mundo:

```sh
docker compose up -d --force-recreate simulator
```

El mundo anterior, `circuit_straight`, conserva sus tres obstaculos en
posiciones pseudoaleatorias reproducibles. Sus cuatro paredes impiden rodear la
pista por fuera y cada distribucion conserva un carril transitable hasta la
meta. Se carga con:

```sh
KAREL_WORLD=circuit_straight docker compose up -d --force-recreate simulator
```

Para mantener esa seleccion en los siguientes arranques, agregar
`KAREL_WORLD=circuit_straight` al archivo `.env`. Para regresar al mundo
predeterminado, eliminar esa linea y volver a crear el servicio `simulator` con
el primer comando.

Abrir:

- Simulador: http://localhost:3000
- Workspace: http://localhost:3001
- Jupyter: http://localhost:3002

Los tres puertos solo escuchan en `127.0.0.1`; no son accesibles desde otros
equipos de la red. Consulte el token generado con
`docker compose logs workspace`; si se define `JUPYTER_TOKEN` en `.env`, use ese
valor al abrir Jupyter.

Dentro del workspace:

```sh
catkin_make
source devel/setup.bash
roslaunch karel_bringup karel_full.launch joystick:=false
```

El bringup abre RViz, la GUI de control y el emulador 68HC11 junto al Gazebo
que se ejecuta en el contenedor `simulator`. El emulador puede omitirse con
`emulator:=false`. El joystick fisico esta desactivado de forma predeterminada
porque Docker Desktop no expone `/dev/input`; en Linux puede habilitarse con un
override que monte el dispositivo y `joystick:=true`.

La ventana `Karel - Control manual` permite avanzar, retroceder y girar
manteniendo pulsado cada boton. `STOP` y la barra espaciadora publican velocidad
cero. `Volver al inicio` reposiciona el modelo en su pose de arranque para el
mundo actual sin modificar el score. El boton separado `Iniciar` comienza la
medicion de tiempo del modulo de evaluacion; el cronometro se detiene al alcanzar
la meta. Tambien muestra odometria, las cuatro distancias ultrasonicas, `/score`
y el ultimo evento de evaluacion.

El boton `LB` del control funciona como habilitador. El stick derecho controla
la velocidad lineal y angular. Los ejes, escalas y boton pueden cambiarse con
los parametros privados de `karel_joystick`.

## Interfaces

| Interfaz                        | Tipo                  | Direccion                               |
| ------------------------------- | --------------------- | --------------------------------------- |
| `/joy`                          | `sensor_msgs/Joy`     | Joystick a teleoperacion                |
| `/hardware/mobile_base/cmd_vel` | `geometry_msgs/Twist` | Teleoperacion a Gazebo                  |
| `/karel/odom`                   | `nav_msgs/Odometry`   | Gazebo a ROS                            |
| `/hardware/ultrasonic`          | `sensor_msgs/Range`   | Ultrasonido frontal                     |
| `/hardware/ultrasonic/back`     | `sensor_msgs/Range`   | Ultrasonido trasero                     |
| `/hardware/ultrasonic/left`     | `sensor_msgs/Range`   | Ultrasonido izquierdo                   |
| `/hardware/ultrasonic/right`    | `sensor_msgs/Range`   | Ultrasonido derecho                     |
| `/score`                        | `std_msgs/Float32`    | Evaluacion a GUI                        |
| `/message`                      | `std_msgs/String`     | Ultimo evento de evaluacion             |
| `/score/detail`                 | `std_msgs/String`     | Desglose de meta, evasiones y contactos |
| `/score/started`                | `std_msgs/Bool`       | Estado de la medicion de score          |
| `/score/elapsed_seconds`        | `std_msgs/Float32`    | Tiempo transcurrido hasta la meta       |

La GUI solicita el inicio con `/karel_score/start` (`std_srvs/Trigger`).
Reposicionar usa `/gazebo/set_model_state` y los parametros
`/karel/initial_pose/{x,y,yaw}` publicados por el lanzamiento del mundo; no
depende del modulo de evaluacion. El servicio existente `/karel_score/reset`
reinicia la evaluacion y deja la medicion detenida.

## Diagnostico

Consultar el estado y los ultimos logs:

```sh
docker compose ps
docker compose logs --tail=100 simulator workspace xserver
```

Jupyter debe aparecer saludable aunque la GUI de Gazebo no pueda renderizar.
El healthcheck del simulador valida ROS y `gzserver`, no `gzclient`. Para una
reconstruccion completamente limpia:

```sh
docker compose down
docker compose build --no-cache simulator workspace
docker compose up -d
```

La masa simulada total es `0.505 kg`. El modelo usa dos ruedas motrices y un
caster pasivo con giro y rodadura independientes. La pista con un obstaculo fijo
es el entorno de evaluacion predeterminado.

El ultrasonido no publica `LaserScan`. La navegacion anterior dependia de una
exploracion angular completa y no debe reactivarse mediante una conversion de
un unico haz.

## Obstaculos en caliente

El mundo `circuit_straight` arranca con tres cajas estaticas en posiciones
aleatorias reproducibles. La semilla inicial puede cambiarse con
`KAREL_OBSTACLE_SEED=<numero>` al arrancar. Sin reiniciar Gazebo se pueden
eliminar o regenerar:

```sh
docker compose exec simulator bash -lc 'source /catkin_ws/devel/setup.bash && rosrun karel_gazebo spawn_circuit_obstacles.py --clear'
docker compose exec simulator bash -lc 'source /catkin_ws/devel/setup.bash && rosrun karel_gazebo spawn_circuit_obstacles.py --seed 9 --count 3'
docker compose exec simulator bash -lc 'source /catkin_ws/devel/setup.bash && rosrun karel_gazebo spawn_circuit_obstacles.py --seed 9 --count 3 --dynamic'
```

Cada regeneracion reinicia la puntuacion mediante `/karel_score/reset`.

## Evaluacion

Los valores se configuran en
`src/karel_evaluation/config/scoring.yaml`:

| Evento                               |      Default |
| ------------------------------------ | -----------: |
| Llegar a la meta                     |           +4 |
| Evadir cada obstaculo sin tocarlo    |           +2 |
| Pasar a 0.20 m o menos sin tocarlo   | +1 adicional |
| Cada nueva colision con un obstaculo |           -3 |

El limite de tiempo predeterminado es de 10 minutos (`time_limit_enabled: true`,
`time_limit_minutes: 10`). En la GUI se puede cambiar la duracion o marcar
«Sin limite de tiempo». Al agotarse, la medicion se detiene y conserva la
puntuacion acumulada; se debe reiniciar la evaluacion antes de iniciar otra.

Un contacto sostenido se penaliza una vez. Si el robot se separa al menos
0.40 s y vuelve a chocar, se considera una colision nueva.

La meta se detecta en cualquier punto del segmento vertical centrado en
`(goal_x, goal_y)`: `goal_line_half_width` define su alcance lateral y
`goal_line_tolerance` la tolerancia en `x` respecto a la linea. No se requiere
detectar el cruce entre muestras.

## Referencia del sensor

La configuracion del ultrasonido adapta el ejemplo oficial para ROS Melodic:

- https://github.com/ros-simulation/gazebo_ros_pkgs/blob/melodic-devel/gazebo_plugins/test/test_worlds/gazebo_ros_range.world

## Licencia

El trabajo propio de este proyecto se distribuye bajo GNU GPL versión 3
exclusivamente (`GPL-3.0-only`); consulte `LICENSE`.

El repositorio también contiene componentes derivados de Toyota y de otros
proyectos que conservan sus licencias y atribuciones originales. La licencia
Clear BSD de Toyota se encuentra en `LICENSE.txt`. El alcance exacto de cada
excepción se documenta en `THIRD_PARTY_NOTICES.md`.
