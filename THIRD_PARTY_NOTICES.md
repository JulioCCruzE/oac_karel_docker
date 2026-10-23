# Avisos de terceros

Salvo donde se indique lo contrario, el trabajo propio de este repositorio se
distribuye bajo `GPL-3.0-only`, cuyo texto completo se encuentra en `LICENSE`.
Las licencias de terceros enumeradas a continuación se conservan y no son
reemplazadas por la licencia principal del proyecto.

## Toyota Motor Corporation

Los siguientes archivos derivan de proyectos de Toyota/HSR y permanecen bajo
`BSD-3-Clause-Clear`, con las modificaciones posteriores identificables en el
historial de Git:

- `docker-compose.yml`
- `dockerfile/entrypoint.sh`
- `pull-images.sh`
- `set-rosmaster.sh`
- `notebooks/1_uso-basico.ipynb`
- `notebooks/2_base_sensores.ipynb`
- `notebooks/utils.py`

El texto aplicable y el aviso de copyright de Toyota se conservan en
`LICENSE.txt`. La inclusión de estos componentes permisivos no cambia la
licencia GPL del trabajo propio ni del programa combinado que los incorpore.

## Gazebo ROS Packages

El bloque del sensor ultrasónico de
`src/karel_gazebo/urdf/karel.xacro` adapta el ejemplo
`gazebo_plugins/test/test_worlds/gazebo_ros_range.world` de
`ros-simulation/gazebo_ros_pkgs` para ROS Melodic. El proyecto de origen lo
publica bajo licencias BSD y Apache 2.0:

<https://github.com/ros-simulation/gazebo_ros_pkgs/blob/melodic-devel/gazebo_plugins/test/test_worlds/gazebo_ros_range.world>

La atribución del origen y sus términos permisivos se mantienen para ese
fragmento; el resto del modelo Karel se distribuye bajo `GPL-3.0-only`.

## Altera Quartus

El archivo `memory/mem_content.mif` contiene
un aviso de Altera Corporation y está sujeto a los términos indicados dentro
del archivo. No se declaran GPL ni BSD. Debe revisarse o regenerarse antes
de una distribución que pretenda estar cubierta íntegramente por GPL.

## Dependencias externas

ROS, Gazebo, Qt, RViz y las imágenes de contenedor referenciadas son proyectos
externos y conservan sus propias licencias. No forman parte del código
relicenciado por este repositorio. Quien redistribuya imágenes binarias debe
conservar los avisos y cumplir las obligaciones de las versiones concretas de
esas dependencias.

La configuración histórica de los contenedores utiliza y adapta elementos de
`devrt/ros-devcontainer-vscode`, publicado bajo Apache License 2.0 por MID
Academic Promotions, Inc. Esta procedencia debe confirmarse antes de una
redistribución; no se ha aplicado una cabecera GPL exclusiva a los Dockerfiles
potencialmente derivados:

<https://github.com/devrt/ros-devcontainer-vscode>
