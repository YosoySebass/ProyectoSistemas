Problema:

Desarrollar un programa en C, ejecutado en una máquina virtual Ubuntu, que procese en paralelo un conjunto de transacciones simuladas (p. ej., consultas a base de datos, procesamiento de archivos o cálculos), utilizando hilos. Medir y analizar los tiempos de ejecución del procesamiento en paralelo (por transacción y total), e incluir una breve discusión de los resultados.


Problemática planteada:

En un laboratorio de matemáticas aplicadas, los investigadores necesitan ejecutar cálculos extremadamente pesados, por ejemplo:
•	Simulaciones numéricas
•	Evaluación de funciones trigonométricas en intervalos gigantes
•	Análisis de series matemáticas
•	Cálculo aproximado de integrales complejas
•	Procesamiento de grandes matrices o señales
Estas tareas toman mucho tiempo si se ejecutan de manera secuencial, por lo que este proyecto utiliza procesamiento paralelo con hilos para distribuir las cargas de trabajo.


Procedimiento para la ejecución:

Dentro del entorno Ubunto mediante compilación con gcc ncluyendo las librerías pthread y math. El proceso inicia con la compilación del código fuente utilizando el comando gcc proyecto.c -o proyecto -pthread -lm seguido de la ejecución con ./proyecto. El sistema configura automáticamente 4 hilos de procesamiento que distribuyen 20 cálculos matemáticos intensivos, donde cada cálculo simula operaciones complejas con funciones trigonométricas y logarítmicas sobre un rango de 50 millones de iteraciones.


Resultados:

Nuestro proyecto generó métricas detalladas del rendimiento que incluyen el tiempo total de procesamiento paralelo (29,119.698 ms), el tiempo secuencial teórico (115,635.255 ms), el tiempo promedio por cálculo matemático (5,781.763 ms) y el tiempo máximo de ejecución individual (6,214.537 ms). Los resultados demuestran una mejora significativa en el rendimiento mediante el uso de paralelismo, donde el tiempo total de ejecución paralela muestra una reducción sustancial de aproximadamente 4 veces en comparación con el enfoque secuencial teórico, evidenciando una eficiente distribución de carga entre los 4 hilos de procesamiento.
Se comprobó la efectividad del mecanismo de sincronización con mutex para proteger el resultado global acumulado (16,727,533,760), garantizando la integridad de los datos durante el procesamiento concurrente de cálculos matemáticos intensivos sin condiciones de carrera. La ejecución finaliza presentando un resumen completo con los valores numéricos obtenidos, donde se destaca la capacidad del sistema para procesar 20 operaciones matemáticas complejas de manera eficiente y coordinada, validando la viabilidad del procesamiento paralelo en entornos de computación científica intensiva.
