# Оптимизация хеш-таблицы с решением коллизий методом цепочек

## Введение

Хеш-таблица — это структура данных, которая обеспечивает быстрый доступ к элементам по ключу. Она использует хеш-функцию — алгоритм, преобразующий ключ (например, строку) в числовой индекс массива, где хранятся данные. Однако, из-за ограниченного размера массива могут возникать коллизии, когда разные ключи получают одинаковый индекс. Для их разрешения в данном проекте применяется **метод цепочек**: каждый элемент массива (бакет) представляет собой связанный список, в котором хранятся все ключи с одинаковым хешем.

Цель проекта — оптимизация производительности хеш-таблицы с использованием оптимизаций (в том числе аппаратно-зависимых) для повышения скорости поиска элементов.

> [!NOTE]
> Если вы хотите сами поработать с хеш-таблицей, то можете сразу переходить к разделу [навигации по проекту](#project_navigation)

## Методика работы

Как будет проходить наша работа?

1. Замеряем производительность хеш-таблицы на тестах.
2. Профилируем и анализируем, ищем узкие места.
3. Делаем попытку оптимизации и снова замеряем производительность.
4. Сравниваем время работы и на основе прироста и погрешности делаем выводы.

Этот несложный список из 4 пунктов лежит в основе проделанной работы, ведь именно по такому алгоритму мы будем проходить каждый раз при написании новой оптимизации.

## Процессор
```
Architecture:             x86_64
  CPU op-mode(s):         32-bit, 64-bit
  Address sizes:          40 bits physical, 48 bits virtual
  Byte Order:             Little Endian
CPU(s):                   4
  On-line CPU(s) list:    0-3
Vendor ID:                GenuineIntel
  BIOS Vendor ID:         QEMU
  Model name:             QEMU Virtual CPU version 4.2.0
    BIOS Model name:      pc-i440fx-bionic  CPU @ 2.0GHz
    BIOS CPU family:      1
    CPU family:           6
    Model:                58
    Thread(s) per core:   1
    Core(s) per socket:   1
    Socket(s):            4
    Stepping:             9
    BogoMIPS:             4200.00
    Flags:                fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology cpuid tsc_kn
                          own_freq pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm cpuid_fault pti ssbd ibrs ibpb
                          fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid rdseed adx smap xsaveopt arat md_clear
Virtualization features:
  Hypervisor vendor:      KVM
  Virtualization type:    full
Caches (sum of all):
  L1d:                    128 KiB (4 instances)
  L1i:                    128 KiB (4 instances)
  L2:                     16 MiB (4 instances)
  L3:                     64 MiB (4 instances)
```

##  Память
```
               total        used        free      shared  buff/cache   available
Mem:           7.8Gi       2.1Gi       4.8Gi        18Mi       1.1Gi       5.6Gi
```
## Описание данных

Данные для проекта взяты из книги **"Хоббит, или Туда и Обратно"**. Из текста были извлечены уникальные слова — всего **7796 слов**, каждое длиной до **29 букв** включительно. Эти слова хранятся в массиве, выровненном по **32 байта**, что улучшает доступ к памяти и совместимость с SIMD-инструкциями.

> [!CAUTION]
> Каждое слово занимает 32 байта в памяти (причем длина слова может быть и меньше, в таком случае оно дополнено нулями до 32 байт)!

## Структура элемента хеш-таблицы

```C
typedef struct string_t{
    char* string;
    uint32_t hash;
    uint32_t length;
} string_t;
```

`string` указывает на слово в массиве данных, `hash` - это ранее посчитанный хеш для данного слова, а `length` - длина этого слова. Индекс элемента в хеш-таблице - это остаток при делении хеша на размер таблицы.

## Структура хеш-таблицы

```C
typedef string_t data_t;
typedef uint32_t (*hash_t)(data_t data);

typedef struct HashMap{
    List* lists;
    uint32_t capacity;
    float load_factor;
    hash_t hash_func;
} HashMap;
```

Хеш-таблица хранит в себе указатель на массив бакетов - `lists`, свой размер - `capacity`, хеш-функцию - `hash_func` и действительный load factor - `load_factor`. **Load factor** - это отношение количества элементов в хеш-таблице к ее размеру. Чем он больше, тем чаще придется искать элементы в списках, а для строк это означает более частое использование `strcmp`.

В данной работе load factor - **7.61**.

Начальный размер хеш-таблицы - **1024**.

> [!NOTE]
> Размер хеш-таблицы не задается простым числом, что может привести к излишнему содержанию элементов в бакете. В данном случае проверено, что распределение произошло равномерно. Все замеры происходили с размером - **1024**.

> [!NOTE]
> Большой load factor был взят намеренно в учебных целях для увеличения числа возможных аппаратных оптимизаций. Обычно в хеш-таблицах используется load factor меньше 1 для уменьшения количества коллизий на бакет и превалирующего поиска элементов по хешу, а не в списках.

> [!CAUTION]
> Автоматического изменения размера в данной реализации хеш-таблицы нет, потому что это требует рехеширования, а это долгая операция, которая может приводить к серьезным простоям.

В хеш-таблице реализованы следующие функции:
- `HashMap hashMapCtor(hash_t hash_func, uint32_t capacity)` : инициализатор хеш-таблицы, принимает указатель на хеш-функцию и изначальный размер хеш-таблицы (в данной работе `BASE_HASH_MAP_CAPACITY`).
- `uint32_t hashMapAddElement(HashMap* hashMap, data_t data)` : добавление элемента в хеш-таблицу, заблаговременно приведенного к `data_t`.
- `uint32_t hashMapPopElement(HashMap* hashMap, data_t data)` : удаление элемента по ключу - поле  `data_t`.
- `bool hashMapSearchElement(HashMap* hashMap, data_t data)` : поиск элемента по ключу - поле `data_t`
- `float getLoadFactor(HashMap* hashMap)` : получение load factor хеш-таблицы.
- `void hashMapDtor(HashMap* hashMap)` : деструктор хеш-таблицы, принимает указатель на таблицу.

Также, если вам необходимо изменить размер хеш-таблицы, реализована функция `HashMap resize(HashMap* hashMap, uint32_t new_capacity)`, которая возвращает новую хеш-таблицу с обновленным размером.

> [!NOTE]
> Так как хеш элемента хранится в структуре, то при вызове `resize` элементы не перехешируются, а просто изменяют свой индекс в хеш-таблице, что ускоряет функцию. Имейте в виду, что ее вызов все равно может приводить к задержкам в работе, так что рекомендуется изначально подобрать оптимальный размер хеш-таблицы.

## Использованные хеш-функции

> [!NOTE]
> Значения crc выбраны эмпирическим путем для уменьшение количества коллизий.

> [!NOTE]
> Хеш-функция crc32HashString(и ее последующие улучшения) реализованы с помощью одного цикла. Напоминаю, что каждое слово занимает по 32 байта (даже если его длина меньше 32, оно просто дополняется до 32 `\0`). Благодаря этому нам не нужно беспокоиться насчет выхода за границы этого слова и писать еще один цикл, который будет замедлять хеширование.

`crc32` с полиномом `0x11EDC6F41ULL` (Castagnoli). [Подробнее можно прочитать здесь](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)

```C
uint32_t crc32HashString(string_t string){
    uint32_t hash  = 0;
    uint32_t chars = 0;
    uint32_t crc   = 0x11111111;
    int remainder  = 0;
    int length     = int(string.length);

    for (int i = 0; i < length - 1; i += 4){
        chars = *(uint32_t*)(string.string + i);
        hash += crc32_u32(crc, chars);
    }

    return hash;
}

static uint32_t bitsReverse(uint32_t num){
    num = ((num & 0xAAAAAAAA) >> 1) | ((num & 0x55555555) << 1);
    num = ((num & 0xCCCCCCCC) >> 2) | ((num & 0x33333333) << 2);
    num = ((num & 0xF0F0F0F0) >> 4) | ((num & 0x0F0F0F0F) << 4);
    num = ((num & 0xFF00FF00) >> 8) | ((num & 0x00FF00FF) << 8);
    num = ((num & 0xFFFF0000) >> 16) | ((num & 0x0000FFFF) << 16);

    return num;
}

static uint32_t crc32_u32(uint32_t crc, uint32_t v){
    uint32_t tmp1 = bitsReverse(v);
    uint32_t tmp2 = bitsReverse(crc);

    uint64_t tmp = ((uint64_t)tmp1 << 32) ^ ((uint64_t)tmp2 << 32);

    uint64_t polynomial = 0x11EDC6F41ULL;

    for (int i = 0; i < 32; i++){
        if (tmp & (1ULL << (63 - i))){
            tmp ^= (polynomial << (31 - i));
        }
    }

    uint32_t result = bitsReverse((uint32_t)tmp);

    return result;
}
```

`murmur2` - второй хеш из семейства murmur. [Подробнее можно прочитать здесь](https://en.wikipedia.org/wiki/MurmurHash)

```C
uint32_t murmur2HashString(string_t string){
    uint32_t num    = 0x5bd1e995;
    uint32_t seed   = 0;
    int32_t  shift  = 24;
    uint32_t length = string.length;
    unsigned char* data = (unsigned char*)string.string;

    uint32_t hash    = seed ^ length;
    uint32_t xor_num = 0;

    while (length >= 4)
    {
        xor_num  = data[0];
        xor_num |= data[1] << 8;
        xor_num |= data[2] << 16;
        xor_num |= data[3] << 24;

        xor_num *= num;
        xor_num ^= xor_num >> shift;
        xor_num *= num;

        hash *= num;
        hash ^= xor_num;

        data += 4;
        length -= 4;
    }

    switch (length)
    {
        case 3: hash ^= data[2] << 16;
        case 2: hash ^= data[1] << 8;
        case 1: hash ^= data[0];
                hash *= num;
    };

    hash ^= hash >> 13;
    hash *= num;
    hash ^= hash >> 15;

    return hash;
}
```

`sum` - хеш-функция, которая просто считывает сумму char в слове.

```C
uint32_t sumHashString(string_t string){
    uint32_t hash  = 0;
    uint32_t chars = 0;
    int remainder  = 0;

    for (; remainder < int(string.length) - 3; remainder += 4){
        chars = *(uint32_t*)(string.string + remainder);
        hash += chars;
    }

    chars = 0;
    for (int i = remainder; i < string.length; i++){
        chars = chars * 256 + string.string[i];
    }
    for (int i = 4 - string.length % 4; i > 0; i--){
        chars *= 256;
    }
    if (chars != 0) hash += chars;

    return hash;
}
```

`adler32` - хеш-функция, разработанная Марком Адлером. Данный алгоритм расчёта контрольной суммы отличается от CRC32 производительностью (adler32 быстрее). [Подробнее можно прочитать здесь](https://en.wikipedia.org/wiki/Adler-32)

```C
uint32_t adler32HashString(string_t string){
    uint32_t A = 1;
    uint32_t B = 0;
    uint32_t mod_adler = 65521;

    for (int i = 0; i < string.length; i++)
    {
        A = (A + string.string[i]) % mod_adler;
        B = (B + A) % mod_adler;
    }

    return (B << 16) | A;
}
```

`elf` - хеш, использованный в Unix ELF формате. [Подробнее можно прочитать здесь](https://en.wikipedia.org/wiki/PJW_hash_function)

```C
uint32_t elfHashString(string_t string){
    uint32_t hash = 0;
    uint32_t high = 0;
    for (int i = 0; i < string.length; i++){
        hash = (hash << 4) + string.string[i];

        if (high = hash & 0xF0000000) hash ^= high >> 24;

        hash &= ~high;
    }
    return hash;
}
```

## Тестирование и оптимизация

**Тест коллизий:** один раз строилась хеш-таблица с определенной хеш-функцией и считалось количество элементов в каждом бакете. Достаточно одного построения, так как коллизии остаются постоянными (если хеш-функция реализована правильно).

**Тест поиска** проводился следующим образом: создавался массив случайных элементов из исходного набора данных, после чего измерялось время поиска этих элементов в хеш-таблице. Ниже описаны этапы оптимизации.

### Шаг 1: Начальное тестирование без флагов

**Тестирование поиска:** 100 тестов по 1000000 поисков.

На первом этапе были протестированы хеш-функции без флагов оптимизации компилятора.
Вот таблица, построенная на основе данных о результатах тестирования хеш-функций:

| Хеш-функция     | **Среднее время поиска (мс)**     | Стандартное отклонение (время, %) | **Коллизии**              |
|-----------------|-----------------------------------|-----------------------------------|---------------------------|
| crc32           | **488349.65 ± 16259.34**          | 3.33                              | **7.61 ± 2.66**           |
| murmur2         | **131433.72 ± 3772.29**           | 2.87                              | **7.61 ± 2.77**           |
| sum             | **251552.22 ± 16589.97**          | 6.60                              | **7.61 ± 15.65**          |
| adler32         | **190853.22 ± 3324.39**           | 1.74                              | **7.61 ± 7.37**           |
| elf             | **746580.99 ± 27929.67**          | 3.74                              | **7.61 ± 33.29**          |

Графики коллизий:

| <div align="center"><img src="./imgs/crc32_collisions.svg" width="100%"></div> | <div align="center"><img src="./imgs/murmur2_collisions.svg" width="100%"></div> |
|-----------------------|------------------------|
| <div align="center"><img src="./imgs/sum_collisions.svg" width="100%"></div>      | <div align="center"><img src="./imgs/adler32_collisions.svg" width="100%"></div> |

<p align="center">
  <img src="./imgs/elf_collisions.svg" width="80%">
</p>

Можно составить топ хеш-функций по (от лучшего к худшему):

- **Времени:**
1. murmur2
2. adler32
3. sum
4. crc32
5. elf

- **Стандартному отклонению коллизий:**
1. crc32
2. murmur2
3. adler32
4. sum
5. elf

Дальше оптимизировать будем, выбрав только одну хеш-функцию. В среднем кажется, что murmur2 - это лучший выбор (она и правда хороша), но для crc32 существуют интринсики, поэтому если `crc32HashString` заменить на:

```C
uint32_t _mm_crc32HashString(string_t string){
    uint32_t hash  = 0;
    uint32_t chars = 0;
    uint32_t crc   = 0x11111111;
    int length     = int(string.length);

    for (int i = 0; i < length - 1; i += 4){
        chars = *(uint32_t*)(string.string + i);
        hash += _mm_crc32_u32(crc, chars);
    }

    return hash;
}
```

то получится та же хеш-функция, но быстрее:

- **Среднее время поиска:** 106695.74 ± 3155.33 мс
- **Стандартное отклонение:** 2.96 %

что сразу поднимает ее на 1-ое место по времени. Самое длинное слово состоит из 29 букв, то есть максимум будет **7 итераций** цикла. Давайте немного изменим функцию и получим фиксированное количество в **4 "итерации"**:

```C
uint32_t _mm_crc32UnrollHashString(string_t string){
    uint32_t hash  = 0;
    uint64_t crc   = 0x1212121121111111;

    uint64_t hash1 = *(uint64_t*)(string.string);
    uint64_t hash2 = *(uint64_t*)(string.string + 8);
    uint64_t hash3 = *(uint64_t*)(string.string + 16);
    uint64_t hash4 = *(uint64_t*)(string.string + 24);


    hash1 = _mm_crc32_u64(crc, hash1);
    hash2 = _mm_crc32_u64(crc, hash2);
    hash3 = _mm_crc32_u64(crc, hash3);
    hash4 = _mm_crc32_u64(crc, hash4);

    hash  = hash1 + hash2 + hash3 + hash4;

    return hash;
}
```

Мы уменьшили количество итераций и совершили развертка цикла, чтобы компилятору было легче применять свои оптимизации, что может привести к заметному ускорению.

- **Среднее время поиска:** 103616.68 ± 2816.31 мс
- **Стандартное отклонение времени:** 2.72 %
- **Стандартное отклонение коллизий**: 2.64

График коллизий `_mm_crc32Unroll`:

<p align="center"> <img src="./imgs/_mm_crc32Unroll_collisions.svg" width="80%"></p>

### Шаг 2: Тестирование с флагом O3

**Тестирование поиска:** 100 тестов по 1000000 поисков.

С использованием флага оптимизации `-O3` производительность улучшилась. Результаты:

| Хеш-функция     | **Среднее время поиска (мс)**     | Стандартное отклонение (время, %) | **Улучшение (%)**      |
|-----------------|-----------------------------------|-----------------------------------|------------------------|
| crc32           | **391546.77 ± 12619.40**          | 3.22                              | **19.84 ± 3.28**       |
| _mm_crc32       | **83277.69 ± 2120.00**            | 2.55                              | **21.97 ± 2.76**       |
| _mm_crc32Unroll | **82388.13 ± 1829.93**            | 2.22                              | **20.48 ± 2.48**       |
| murmur2         | **93346.97 ± 1619.66**            | 1.74                              | **28.97 ± 2.31**       |
| sum             | **183716.68 ± 6176.15**           | 3.36                              | **26.97 ± 4.98**       |
| adler32         | **132540.09 ± 4137.47**           | 3.12                              | **30.56 ± 2.53**       |
| elf             | **529455.36 ± 12735.30**          | 2.41                              | **29.10 ± 3.54**       |

### Шаг 3: Выбор хеш-функции

На основе результатов была выбрана функция `_mm_crc32Unroll` как наиболее быстрая и c наименьшим стандартным отклонением по коллизиям.

### Шаг 4: Анализ с помощью Valgrind

Профилирование с помощью Valgrind показало, что основным узким местом является функция `strcmp`. Это связано с частыми сравнениями строк при поиске в цепочках.

> [!NOTE]
> Время работы каждой функции указано в режиме self (без учета вызывающихся из нее функций)

```Valgrind
--------------------------------------------------------------------------------
Ir                    file:function
--------------------------------------------------------------------------------
973,521,544 (42.67%)  ./string/../sysdeps/x86_64/multiarch/strcmp-avx2.S:__strcmp_avx2 [/usr/lib/x86_64-linux-gnu/libc.so.6]
742,238,480 (32.53%)  src/list.cpp:searchElement(List*, string_t) [/root/hashMap/build/main]
260,000,000 (11.40%)  src/hash_map.cpp:hashMapSearchElement(HashMap*, string_t) [/root/hashMap/build/main]
```

### Шаг 5: Оптимизация strcmp

**Тестирование поиска:** 1000 тестов по 1000000 поисков.

Для устранения узкого места была разработана собственная функция `my_strcmp`, использующая AVX2-инструкции:

```asm
my_strcmp:
    vmovdqa ymm0, [rdi]
    vmovdqa ymm1, [rsi]

    vpcmpeqq ymm2, ymm0, ymm1
    vpmovmskb eax, ymm2

    not eax

    ret
```

Objdump показывает, что новая реализация strcmp используется при сравнении строк
```asm
0000000000003b40 <my_strcmp>:
    3b40:	c5 fd 6f 07          	vmovdqa ymm0,YMMWORD PTR [rdi]
    3b44:	c5 fd 6f 0e          	vmovdqa ymm1,YMMWORD PTR [rsi]
    3b48:	c4 e2 7d 29 d1       	vpcmpeqq ymm2,ymm0,ymm1
    3b4d:	c5 fd d7 c2          	vpmovmskb eax,ymm2
    3b51:	f7 d0                	not    eax
    3b53:	c3                   	ret
```

Все слова занимают ровно по 32 байта, поэтому умещаются в регистры `ymm`, причем они выровнены, что повзоляет использовать версию инструкций для выровненных данных, соответственно ускоряя работу функции.

Результаты после оптимизации: среднее время поиска снизилось до **61938.46 ± 2322.45 мс**, улучшение на **24.82 ± 3.08 %**.

### Шаг 6: Инлайнинг функции хеширования

Профилируем еще раз с помощью Valgrind:

```Valgrind
--------------------------------------------------------------------------------
Ir                    file:function
--------------------------------------------------------------------------------
694,551,940 (44.90%)  src/list.cpp:searchElement(List*, string_t) [/root/hashMap/build/main]
286,981,092 (18.55%)  ???:0x0000000000003b40 [/root/hashMap/build/main]
260,000,000 (16.81%)  src/hash_map.cpp:hashMapSearchElement(HashMap*, string_t) [/root/hashMap/build/main]
```

`strcmp` исчез, но появилась неизвестная функция `???:0x0000000000003b40`. Если сопоставить адрес (3b40), то можно понять, что это новый `my_strcmp`.

Функция `searchElement` выглядит так:

```C
int searchElement(List* list, data_t data){
    assert(list);

    uint32_t inx = list->list_elems[0].next_inx;

    while (inx != 0){
        if (cmpListElems(&data, &list->list_elems[inx].data) == 0) return inx;

        inx = list->list_elems[inx].next_inx;
    }

    return inx;
}
```

причем `cmpListElems` на самом деле инлайнится и по итогу просто вызывается `my_strcmp`:

```asm
0000000000002190 <_Z13searchElementP4List8string_t>:
    ...
    21cc:	48 8b 74 18 10       	mov    rsi,QWORD PTR [rax+rbx*1+0x10]
    21d1:	e8 6a 19 00 00       	call   3b40 <my_strcmp>
    21d6:	85 c0                	test   eax,eax
    ...
```

По моему мнению, пока ничего особо здесь и не прооптимизируешь, так что двинемся дальше: к функции `hashMapSearchElement`:

```C
bool hashMapSearchElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    uint32_t supposed_index = hashMap->hash_func(data) % hashMap->capacity;

    uint32_t inx = searchElement(&hashMap->lists[supposed_index], data);

    if (inx == 0) return false;

    return true;
}
```

Как видно, хеш-функция вызывается по указателю в качестве поля структуры HashMap. Это приводит к зависимости по данным, так как чтобы перейти по адресу функции его сначала нужно выгрузить из памяти:

```asm
0000000000001860 <_Z20hashMapSearchElementP7HashMap8string_t>:
    ...
    1879:	48 89 d6             	mov    rsi,rdx
    187c:	ff 53 10             	call   QWORD PTR [rbx+0x10]
    187f:	31 d2                	xor    edx,edx
    ...
```

Решим эту проблему ручным инлайнингом хеш-функции:

```C
bool hashMapSearchElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    uint32_t hash  = 0;
    uint32_t chars = 0;
    uint64_t crc   = 0x1212121121111111;

    uint64_t hash1 = *(uint64_t*)(data.string);
    uint64_t hash2 = *(uint64_t*)(data.string + 8);
    uint64_t hash3 = *(uint64_t*)(data.string + 16);
    uint64_t hash4 = *(uint64_t*)(data.string + 24);


    hash1 = _mm_crc32_u64(crc, hash1);
    hash2 = _mm_crc32_u64(crc, hash2);
    hash3 = _mm_crc32_u64(crc, hash3);
    hash4 = _mm_crc32_u64(crc, hash4);

    hash  = hash1 + hash2 + hash3 + hash4;

    uint32_t supposed_index = hash % hashMap->capacity;

    uint32_t inx = searchElement(&hashMap->lists[supposed_index], data);

    if (inx == 0) return false;

    return true;
}
```

Результаты: среднее время поиска — **62929.24 ± 2226.83 мс**. Время не улучшилось, а даже ухудшилось. Проведем еще несколько тестов:

### Таблица 1: Результаты без оптимизации инлайна хеш-функции
| Итерация | **Среднее время поиска (мс)**     | Стандартное отклонение (время, %) |
|----------|-----------------------------------|-----------------------------------|
| 1        | **62132.26 ± 1658.15**            | 2.67                              |
| 2        | **61769.82 ± 2013.93**            | 3.26                              |
| 3        | **62952.78 ± 1711.90**            | 2.72                              |
| 4        | **61277.61 ± 1604.06**            | 2.62                              |
| 5        | **61699.78 ± 887.18**             | 1.44                              |

### Таблица 2: Результаты с оптимизацией инлайна хеш-функции
| Итерация | **Среднее время поиска (мс)**     | Стандартное отклонение (время, %) | **Улучшение времени (%)**     |
|----------|-----------------------------------|-----------------------------------|-------------------------------|
| 1        | **61802.47 ± 1092.90**            | 1.77                              | **0.53 ± 2.23**               |
| 2        | **61364.97 ± 1586.20**            | 2.58                              | **0.66 ± 2.93**               |
| 3        | **61004.51 ± 1313.10**            | 2.15                              | **3.09 ± 2.44**               |
| 4        | **61565.20 ± 1657.83**            | 2.69                              | **-0.47 ± 2.66**              |
| 5        | **61603.65 ± 1959.32**            | 3.18                              | **0.16 ± 2.34**               |

Как видно, почти все улучшения не выходят за предел отклонения - это значит, что мы не можем говорить о приросте производительности. Давайте будем прогонять каждую хеш-функцию по 100 раз:

```
        volatile uint32_t supposed_index = 0;
        for (int i = 0; i < 100; i++){
            supposed_index = hashMap->hash_func(data) % hashMap->capacity;
        }
```

Как видно, компилятор оставил этот цикл (смотрим по objdump, итерационный регистр - ebp):
```
    168c:	bd 64 00 00 00       	mov    $0x64,%ebp
    1691:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%rsp)
    1698:	00
    1699:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
    16a0:	4c 89 ef             	mov    %r13,%rdi
    16a3:	4c 89 e6             	mov    %r12,%rsi
    16a6:	ff 53 10             	call   *0x10(%rbx)
    16a9:	31 d2                	xor    %edx,%edx
    16ab:	f7 73 08             	divl   0x8(%rbx)
    16ae:	89 54 24 0c          	mov    %edx,0xc(%rsp)
    16b2:	83 ed 01             	sub    $0x1,%ebp
    16b5:	75 e9                	jne    16a0 <_Z20hashMapSearchElementP7HashMap8string_t+0x30>
```

Результаты для 100 тестов:

- **Среднее время поиска:** 374951.24 ± 18499.76 мс
- **Стандартное отклонение времени:** 4.93 %

Подключим инлайнинг хеш-функции:
```
        volatile uint32_t hash  = 0;
        volatile uint64_t crc   = 0x1212121121111111;
        for (int i = 0; i < 100; i++){
            hash  = 0;

            uint64_t hash1 = *(uint64_t*)(data.string);
            uint64_t hash2 = *(uint64_t*)(data.string + 8);
            uint64_t hash3 = *(uint64_t*)(data.string + 16);
            uint64_t hash4 = *(uint64_t*)(data.string + 24);

            hash1 = _mm_crc32_u64(crc, hash1);
            hash2 = _mm_crc32_u64(crc, hash2);
            hash3 = _mm_crc32_u64(crc, hash3);
            hash4 = _mm_crc32_u64(crc, hash4);

            hash  = hash1 + hash2 + hash3 + hash4;
        }
```

Цикл остался в коде (итерационный регистр - ecx):
```
    16a1:	b9 64 00 00 00       	mov    $0x64,%ecx
    16a6:	48 b8 11 11 11 21 11 	movabs $0x1212121121111111,%rax
    16ad:	12 12 12
    16b0:	48 8b 76 18          	mov    0x18(%rsi),%rsi
    16b4:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
    16b9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
    16c0:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%rsp)
    16c7:	00
    16c8:	48 8b 44 24 08       	mov    0x8(%rsp),%rax
    16cd:	4c 8b 64 24 08       	mov    0x8(%rsp),%r12
    16d2:	48 8b 6c 24 08       	mov    0x8(%rsp),%rbp
    16d7:	f2 48 0f 38 f1 c3    	crc32  %rbx,%rax
    16dd:	48 8b 54 24 08       	mov    0x8(%rsp),%rdx
    16e2:	f2 4d 0f 38 f1 e3    	crc32  %r11,%r12
    16e8:	f2 48 0f 38 f1 ef    	crc32  %rdi,%rbp
    16ee:	44 01 e0             	add    %r12d,%eax
    16f1:	f2 48 0f 38 f1 d6    	crc32  %rsi,%rdx
    16f7:	01 e8                	add    %ebp,%eax
    16f9:	01 d0                	add    %edx,%eax
    16fb:	89 44 24 04          	mov    %eax,0x4(%rsp)
    16ff:	83 e9 01             	sub    $0x1,%ecx
    1702:	75 bc                	jne    16c0 <_Z20hashMapSearchElementP7HashMap8string_t+0x50>
```

Результаты для 100 тестов:

- **Среднее время поиска:** 218821.73 ± 15791.76 мс
- **Стандартное отклонение времени:** 7.22 %
- **Улучшение:** 41.64 ± 6.18 %

Здесь улучшение явно проглядывается, так что в случае, когда хеш функция вызывается чаще, чем в нашем проекте, инлайнинг - это отличная оптимизации. Если же искусственно не увеличивать число вызова хеш-функции, то прироста производительности нет.

### Шаг 7: Оптимизация остатка от деления

В той же функции `hashMapSearchElement` мы находим остаток от `hashMap->capacity`, чтобы получить индекс элемента, который подойдет для хеш-таблицы. Но изначально хеш-таблица задается с помощью `capacity = 1024`. Тогда можно заменить дорогостоящую операцию деления (`div`) на побитовый и (`and`).

- Было: `uint32_t supposed_index = hashMap->hash_func(data) % hashMap->capacity;`

```asm
    1881:	4c 89 e6             	mov    rsi,r12
    1884:	f7 73 08             	div    DWORD PTR [rbx+0x8]
    1887:	48 8b 03             	mov    rax,QWORD PTR [rbx]
```

- Стало: `uint32_t supposed_index = hashMap->hash_func(data) & (hashMap->capacity - 1);`

```asm
    18a3:	83 ea 01             	sub    edx,0x1
    18a6:	21 d0                	and    eax,edx
    18a8:	48 8d 14 40          	lea    rdx,[rax+rax*2]
```

Результаты: среднее время поиска — **57949.40 ± 2911.72 мс**, улучшение на **7.91 ± 4.43 %**.

### Шаг 8: Инлайнинг my_strcmp

Вернемся к функции `searchElement`. Больше всего инструкций исполняется в этой функции (по Valgrind), поэтому осмотрим ее еще раз:

```asm
00000000000021b0 <_Z13searchElementP4List8string_t>:
    ...
    21ec:	48 8b 74 18 10       	mov    rsi,QWORD PTR [rax+rbx*1+0x10]
    21f1:	e8 6a 19 00 00       	call   3b60 <my_strcmp>
    21f6:	85 c0                	test   eax,eax
    ...
```

Из долгих функций здесь присутствует call на my_strcmp. Сначала мы сделали отдельную ассемблерную функцию, теперь пришло время вручную заинлайнить ее. Используем ассемблерные вставки:

```c
asm volatile (
    "vmovdqa (%1), %%ymm0\n\t"
    "vmovdqa (%2), %%ymm1\n\t"
    "vpcmpeqb %%ymm1, %%ymm0, %%ymm2\n\t"
    "vpmovmskb %%ymm2, %0\n\t"
    "not %0\n\t"
    : "=r"(result)
    : "r"(data1->string), "r"(data2->string)
    : "ymm0", "ymm1", "ymm2"
);
```

Результаты: среднее время поиска — **53345.03 ± 3017.11 мс**, улучшение на **7.95 ± 5.34 %**.

На этом моменте я решил остановиться, потому что поиск остальных оптимизаций уже будет занимать много дольше времени и давать меньшие проценты.

### Шаг 9: Замена searchElement на asm вставку (остановка оптимизаций)

Функция searchElement стоит в Valgrind на 1 месте, поэтому было бы славно ее оптимизировать. Можно переписать ее через ассемблерную вставку в функции hashMapSearchElement. Для начала проанализируем код searchElement в objdump:
```
0000000000002160 <_Z13searchElementP4List8string_t>:
    2160:	f3 0f 1e fa          	endbr64
    2164:	48 85 ff             	test   rdi,rdi
    2167:	74 43                	je     21ac <_Z13searchElementP4List8string_t+0x4c>
    2169:	48 8b 3f             	mov    rdi,QWORD PTR [rdi]
    216c:	8b 07                	mov    eax,DWORD PTR [rdi]
    216e:	85 c0                	test   eax,eax
    2170:	75 0c                	jne    217e <_Z13searchElementP4List8string_t+0x1e>
    2172:	eb 35                	jmp    21a9 <_Z13searchElementP4List8string_t+0x49>
    2174:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    2178:	8b 02                	mov    eax,DWORD PTR [rdx]
    217a:	85 c0                	test   eax,eax
    217c:	74 2a                	je     21a8 <_Z13searchElementP4List8string_t+0x48>
    217e:	89 c2                	mov    edx,eax
    2180:	48 c1 e2 05          	shl    rdx,0x5
    2184:	48 01 fa             	add    rdx,rdi
    2187:	48 8b 4a 10          	mov    rcx,QWORD PTR [rdx+0x10]
    218b:	c5 fd 6f 06          	vmovdqa ymm0,YMMWORD PTR [rsi]
    218f:	c5 fd 6f 09          	vmovdqa ymm1,YMMWORD PTR [rcx]
    2193:	c5 fd 74 d1          	vpcmpeqb ymm2,ymm0,ymm1
    2197:	c5 fd d7 ca          	vpmovmskb ecx,ymm2
    219b:	f7 d1                	not    ecx
    219d:	85 c9                	test   ecx,ecx
    219f:	75 d7                	jne    2178 <_Z13searchElementP4List8string_t+0x18>
    21a1:	c3                   	ret
    21a2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    21a8:	c3                   	ret
    21a9:	31 c0                	xor    eax,eax
    21ab:	c3                   	ret
    21ac:	50                   	push   rax
    21ad:	e8 0e fd ff ff       	call   1ec0 <_Z13searchElementP4List8string_t.part.0>
    21b2:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
    21b9:	00 00 00 00
    21bd:	0f 1f 00             	nop    DWORD PTR [rax]
```

После просмотра objdump я пришел к выводу, что вряд ли получится написать что-то быстрее компиляторной версии или на это уйдет слишком много времени, поэтому оставим функцию такой, какая она есть. Просмотрим еще раз Valgrind:

```
717,611,560 (61.16%)  src/list.cpp:searchElement(List*, string_t) [/root/hashMap/build/main]
200,000,000 (17.05%)  src/hash_map.cpp:hashMapSearchElement(HashMap*, string_t) [/root/hashMap/build/main]
108,054,941 ( 9.21%)  src/tests.cpp:testSearchTime(_IO_FILE*, unsigned int (*)(string_t), TestData, unsigned int) [/root/hashMap/build/main]
```

Дальше идет только оптимизация функций тестирования, что не входит в цель данного проекта, поэтому я принял решение на этом закончить.

### Шаг 10 (бонус): Замена char* в data_t на __m256i

Все это время мы работали со строками в их явном понимании: массив символов. Но мы знаем, что каждое слово в нашем наборе не больше 32 байт, так почему бы не заменить char* на _m256i (32 байтный тип данных)?

Результаты (со всеми остальными оптимизациями):

Cреднее время поиска — **65569.22 ± 5610.22 мс**

Время ухудшилось. Почему? Скорее всего из-за того, что для выравнивания структур используется дополнительная память, что сильно ухудшает кеши, а так же мизерных изменений по сравнению с char*.

strcmp с __m256i:
```
    asm volatile (
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpeqb %%ymm1, %%ymm0, %%ymm2\n\t"
        "vpmovmskb %%ymm2, %0\n\t"
        "not %0\n\t"
        : "=r"(result)
        : "x"(data1->string), "x"(data2->string)
        : "ymm0", "ymm1", "ymm2"
    );
```
strcmp c char*:
```
        asm volatile (
            "vmovdqa (%1), %%ymm0\n\t"
            "vmovdqa (%2), %%ymm1\n\t"
            "vpcmpeqb %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpmovmskb %%ymm2, %0\n\t"
            "not %0\n\t"
            : "=r"(result)
            : "r"(data1->string), "r"(data2->string)
            : "ymm0", "ymm1", "ymm2"
        );
```

Они оба инлайнятся.

## Результаты и выводы

| Этап                        | Среднее время поиска (мс) | Улучшение (%) |
|-----------------------------|---------------------------|---------------|
| Без флагов (_mm_crc32Unroll)| 103616.68                 | -             |
| С O3                        | 82388.13                  | 20.48         |
| Оптимизация strcmp          | 61938.46                  | 24.82         |
| Инлайнинг хеш-функции       | 62929.24                  | -             |
| Оптимизация div → and       | 57949.40                  | 7.91          |
| Инлайнинг my_strcmp         | 53345.03                  | 7.95          |

Наиболее значительный прирост производительности обеспечили:
1. Оптимизация компилятором с флагом `-O3` (20.48%).
2. Замена `strcmp` на `my_strcmp` (24.82%).
3. Замена деления на побитовую операцию (7.91%) и инлайнинг `my_strcmp` (7.95%).

По итогу мы смогли ускорить поиск в хеш-таблице на **48.52%**.

Проект демонстрирует, как аппаратно-зависимые оптимизации и низкоуровневые улучшения могут значительно повысить производительность хеш-таблицы.

<a id="project_navigation"></a>
## Навигация по проекту

Чтобы работать с реализованной хеш-таблицей, сначала склонируйте репозиторий к себе на машину:
```
git clone https://github.com/whiterno/hashMap.git
```

Сборка проекта производится с помощью `Cmake`, для удобной компиляции и проведения тестов созданы скрипты на `bash`: `compile.sh` и `tests.sh`.

При сборке вы можете использовать флаги компиляции, которые могут включать/выключать некоторые оптимизации:

- `O3` - при включении этого флага проект будет собираться с -O3.
- `SEARCH_ONLY` - флаг для тестирования, замеры будут происходить только на поисках.
- `STRCMP_OPTIMIZATION` - флаг, включающий оптимизацию strcmp. Работает для строк длины < 32, так как для сравнения используется по одному ymm регистру.
- `STRCMP_OPTIMIZATION_INLINE` - флаг, который инлайнит оптимизированный strcmp.
- `CRC32_OPTIMIZATION` - флаг, который инлайнит _mm_crc32Unroll в функцию поиска. При включении этого флага хеш-функция, которая хранится в структуре таблицы не используется.
- `REMAINDER_OPTIMIZATION` - флаг, включающий оптимизации деления с остатком. Чтобы он правильно работал, размер хеш-таблицы должен быть степенью двойки (2, 4, 8, ...).

Чтобы включить оптимизации, достаточно прописать:
```
./compile.sh -D<compile_option1>=ON  -D<compile_option2>=ON ... -D<compile_optionN>=ON
```

Не прописанные флаги по умолчанию имеют значение `OFF`.

### Тестирование

Если вы хотите провести тестирование на своей машине, реализован скрипт `tests.sh`.
```
./tests.sh <tests_amount> <hash_function> <is_plot_needed> [<compile_options>]
```

- `<tests_amount>` - количество тестов, которые необходимо провести.
- `<hash_function>` - название хеш-функции или `all`, чтобы провести тесты для всех. Название хеш-функций для аргумента:
- - `crc32`
- - `_mm_crc32`
- - `_mm_crc32Unroll`
- - `murmur2`
- - `sum`
- - `adler32`
- - `elf`
- `<is_plot_needed>` - строить ли график для тестов. `plot`, чтобы строить, любое другое значение не будет восприниматься.

Графики будет храниться в папке `plot`.
