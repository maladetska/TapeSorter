# TapeSorter

### How to build and start?

Config example:
```
$ cat <CONFIG_DIR>/config.yaml
N = 19
M = 65
delay_for_read = 0
delay_for_put = 0
delay_for_shift = 0
path_in = <PATH_TO_INPUT_TAPE>
path_out = <PATH_TO_OUTPUT_TAPE>
```

Commands:
```
$ git clone 'https://github.com/maladetska/TapeSorter'
$ cd TapeSorter
$ ./launch.sh <CONFIG_DIR>/config.yaml
```

Для отдельного запуска ТЕСТОВ (из ./TapeSorter):
```
$ ./launch_tests.sh
```

Для запуска примера (из ./TapeSorter):
```
$ ./launch_example.sh
```
или
```
$ ./launch.sh ../example/config3.yaml
```


### Идея:
Условимся, что внешней памятью будем считать файлы, в которых лежат ленты ( $Tape$ ). 
Если $M <= N$ или $M$ не сильно превышает занчение $N$ , тогда лента полностью может не поместиться во внутреннюю память, поэтому будем делить ленту на "куски" ( $Chunk$ ) и работать во внутренней памяти только с одним куском. Уже с кусками работать удобнее и памяти должно хватить, если правильно рассчитать количество таких кусков ( $ChunksInfo.count$ _ $of$ _ $chunks$ ) и их размеры ( $ChunksInfo.max$ _ $size$ _ $chunk$ и $ChunksInfo.last$ _ $size$ _ $chunk$ ) (об этом позже).

Итак, чтобы отсортировать ленту, нужно в первую очередь разделить её полностью на ленты. Размер одной ленты и одного куска будут совпадать, то прямо во внутренней памяти отсортируем ( $std::sort$ ) ленты (то есть куски) и положим их во внешнюю память.(1) Полученных лент будет $ChunksInfo.count$ _ $of$ _ $chunks$ ; размер всех, кроме последней, - $ChunksInfo.max$ _ $size$ _ $chunk$ ; размер последней - $ChunksInfo.last$ _ $size$ _ $chunk$ или $ChunksInfo.max$ _ $size$ _ $chunk$ . Теперь будем попарным слиянием лент получать ленты бОльшего размера(2), пока не получим одну отсортированную ленту, которую поместим в ленту-ответ.

Как же определиться с размером одного куска? Внутренняя память нужна для сортировки ленты-куска(1) и для слияние далее полученных лент(2). 
- Во внутренней памяти есть $M$ свободных байт;
- Размер числа на ленте не превышает 4 байта ( $int32 _t$ -> 32 бита);
Для первого случая учтём, что:
- $std::sort$ работает за $n$ памяти;
- пусть размер куска $Chunk.size$;
- внутреннюю память надо выделить на кусок ( $Chunk.size$ ), $std::sort$ ( $Chunk.size$ ) и на отсортированный кусок ( $Chunk.size$ ), а так же оставить на хранения некоторых перемнных и выполнение некоторых операций (пусть тоже $Chunk.size$ );
- нам понадрбится $4 * Chunk.size$.
Для второго случая учтём, что:
- пусть размер куска $Chunk.size$;
- внутреннюю память надо выделить на два куска, которые мы будем слиять ($Chunk.size$ + $Chunk.size$) и на кусок новой ленты, который будем заполнять ($Chunk.size$), а так же оставить на хранения некоторых перемнных и выполнение некоторых операций (пусть тоже $Chunk.size$);
- нам понадрбится $4 * Chunk.size$.

Итак, в двух случаях нам надо выделить $4 * Chunk.size == M / 4$ => $Chunk.size == M / 16$
