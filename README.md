PAV - P3: estimación de pitch
=============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.
    ```c++
      void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

        for (unsigned int l = 0; l < r.size(); l++) {

          r[l] = 0;

          for (unsigned int n = l; n < x.size(); n++)
            r[l] += x[n]*x[n-l];

          r[l] /= x.size();
        }

        if (r[0] == 0.0F)
          r[0] = 1e-10; 
      }
    ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.

    <img src="img/p3_grafica_autocorr.png" width="800" align="center">

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la biblioteca matplotlib de Python.

   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
      ```c++
      float PitchAnalyzer::compute_pitch(vector<float> & x) const {
        if (x.size() != frameLen)
          return -1.0F;

        for (unsigned int i=0; i<x.size(); ++i)
          x[i] *= window[i];

        vector<float> r(npitch_max);

        autocorrelation(x, r);

        vector<float>::const_iterator iR = r.begin(), iRMax = iR;

        for (iR = iRMax = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++)
          if (*iR > *iRMax) iRMax = iR;

        unsigned int lag = iRMax - r.begin();

        float pot = 10 * log10(r[0]);
        
        if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]))
          return 0;
        else
          return (float) samplingFreq/(float) lag;
      }
      ```
   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.

      ```c++
      bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
        if (rmaxnorm > umaxnorm && r1norm > ur1norm) return false;
        return true;
      }
      ```
- Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la estimación de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

      <img src="img/p3_pitch_2.png" width="800" align="center">
      <img src="img/p3_autocorrelacio.png" width="800" align="center">
      ***Observamos como tanto en la potencia como en r1norm y en rmaxnorm aparecen máximos en los tramos sonoros y mínimos en los tramos sordos. También podemos ver como un buen umbral de decisión para r1norm y rmaxnorm estará alrededor de 0.4, mientras que para la potencia rondará los 30-40 dB. Hemos decidido que no usaremos la tasa de cruces por cero, ya que consideramos que no es demasiado relevante.***
      

      - Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos estimadores.

     ***Pitch calculado por wavesurfer:***
     <img src="img/p3_pitch_ref.png" width="800" align="center">

     ***Pitch calculado por nuestro programa get_pitch:***
     <img src="img/p3_resultat_get_pitch.png" width="800" align="center">

     ***Comparación:***
     <img src="img/p3_comparacio.png" width="800" align="center">

		Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará
	 	el uso de alternativas de mayor calidad (particularmente Python).
  
  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

    ***Score final con los ejercicios de ampliación:***
    <img src="img/p3_score_final.png" width="800" align="center">

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

    ***Mensaje de ayuda:***
    <img src="img/p3_help_docopt.png" width="800" align="center">

    ***Código de run_get_pitch.sh (hemos añadido un if que hace que si no se entran parámetros se utilizen unos por defecto):***
    <img src="img/p3_bash.png" width="800" align="center">

    ***Ejemplo de utilización:***
    <img src="img/p3_utilitzacio_parametres.png" width="800" align="center">

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al estimador. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
  ***Hemos realizado las siguientes mejoras de preprocesado y post procesado***

  ***Normalización y center clipping tanto de la señal como de la autocorrelación (preprocesado):***

  ```c++
  ///Normalitzar la senyal i aplicar center clipping
  float max_senyal = 0;
  for (unsigned int i = 0; i < x.size(); i++)
    if (x[i] > max_senyal)
      max_senyal = x[i];
  float th_cp = ucenterclipping;
  for (unsigned int i = 0; i < x.size(); i++) {
    x[i] = x[i] / max_senyal;
    if (abs(x[i]) < th_cp)
      x[i] = 0;
    if (x[i] > th_cp)
      x[i] -= th_cp;
    if (x[i] < -th_cp)
      x[i] += th_cp;
  }
  ```

  ```c++
  //Normalitzar autocorrelació i aplicar center clipping
  float max_r = *max_element(r.begin(), r.end());
  float th_cp_r = 0.1;
  for (unsigned int i = 0; i < r.size(); i++) {
    r[i] = r[i] / max_r;
    if (abs(r[i]) < th_cp_r)
      r[i] = 0;
    if (r[i] > th_cp_r)
      r[i] -= th_cp_r;
    if (r[i] < -th_cp_r)
      r[i] += th_cp_r;
  }
  ```

  ***Filtro de mediana de grado 3 no recursivo (postprocesado):***

  ```c++
  //Filtre de mediana
  std::vector<float> f0_aux(f0);
  float max;
  float min;

  for (unsigned int j = 1; j < f0_aux.size() - 1; j++) {
    max = 0;
    min = 500;
    for (int i = -1; i < 2; i++) {
      if (f0_aux[j+i] > max)
        max = f0_aux[j+i];
      if (f0_aux[j+i] < min)
        min = f0_aux[j+i];
    }
    f0[j] = f0_aux[j-1] + f0_aux[j] + f0_aux[j+1] - max - min;
  }
  ```

  ***Optimización de parámetros. Al final no utilizamos la potencia para el decisor de voiced/unvoiced y entramos como 3r parámetro el umbral de center clipping de la señal, y determinamos que los parámetros óptimos son umaxnorm=0.29  ur1norm=0.36  y ucenterclipping=0.02, con los que hemos optenido una evaluación total de 91.23 %***

  <img src="img/p3_optimitzacio_1.png" width="800" align="center">
  <img src="img/p3_optimitzacio_2.png" width="800" align="center">

  ***Cepstrum. Intentamos implementarlo pero nos daba errores que no hemos sido capaces de solucionar a tiempo, así que al final utilizamos autocorrelación***

  ```c++
  //intent cepstrum
  void PitchAnalyzer::cepstrum(const vector<float> &x, vector<float> &c) const {
    float a = log2(x.size());
    unsigned int n = ceil(a);
    long len = 2^n;
    ffft::FFTReal <float> fft_object (len);

    float x2 [len];
    float f [len];
    float f_log [len];
    float cep [len];

    for (unsigned int i = 0; i < x.size(); i++)
      x2[i] = x[i];
    fft_object.do_fft(f, x2);
    for (unsigned int i = 0; i < len; i++)
      f_log[i] = log10(abs(f[i]));
    fft_object.do_ifft(f_log, cep);
    for (unsigned int i = 0; i < c.size(); i++)
      c[i] = cep[i];
  }
  ```
Evaluación *ciega* del estimador
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
estimador con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
