# Reverse | hard | Super Secure Linux (rev-part)

## Информация

> Наши агенты используют только сверх-защищённые операционные системы
> 
> Но, кажется, мы подвергилсь атаке на поставщика и нам внедрили бэкдоры.
> 
> У нас есть базовый образ, который мы ставим на все компьюетры, посмотрим может что-то найдёшь.
>  
>
> *Файлы для данного задания аналогичны файлам задания категории "Forensics" с названием "Super Secure Linux (for-part)". Если вы нашли флаг и он не подходит к этому таску, попробуйте сдать его в другую задачу. В данной задаче нужно разобрать с бэкдором.*
> *md5(flag) == 56361cb66d9cb9f61fb8ab6440df5e32*
> 

## Деплой

Не требуется

## Выдать участинкам

Архив из директории [public/](public/)

## Описание

Ядро Linux с некоторыми модификациями и бекдором

## Решение

1. Распаковываем архив из задания и видим, что нам дан образ под qemu
2. Запустим образ и посмотрим на ФС и процессы
3. Ничего подозрительного не находится, посмотрим `dmesg`. Также пусто и нет никаких модулей ядра
4. Данные факты могут натолкнуть нас на мысли о том, что бэкдор вшит прямо в ядро
5. Распакуем данную нам ФС и посмотрим на файлы
6. Если внимательно изучить содержимое файла `/etc/inittab` в базовом образе ФС и в запущенной системе, то можно обнаружить, что они разные
7. Так-как этот файл используется для запуска команд в процессе старта ядра, через него могли внедряться бэкдорные команды
8. Распакуем ядро и загрузим его в IDA PRO, также обработаем символьную информацию, чтобы удобнее проводить анализ
9. Символы ядра можно восстановить с помощью данных [скриптов](https://github.com/mephi42/ida-kallsyms)
10. Находим в базе строчку `/etc/inittab` и перекрёстные ссылки на неё, их всего две: одна в функции `do_name`, вторая в `do_execve`
11. Анализирую функцию `do_name` можно найти, что в ней изменяется `/etc/inittab` и в него дописывается дополнительная команда запуска некоторого исполняемого файла
12. Файл вызывает по следующему пути `/tHVVHY5d/RbFA989N`, но такой директории и файла нет на системе при её запуске
13. Однако, такая директория существует, просто она скрыта с помощью модификации ядра
14. Зайдя в директорию видем в ней два файла: исполняемый и некоторый бинарный файл
15. Бинарный файл `FxGjGjMh` служит конфигом для бекдора и содержит в себе необходимые данные для решения таска из форензики (но его можно получить и другим способом)
16. Конфиг зашифрован и имеет некоторый простой формат, ключ шифрования хранится в самом конфиге
17. Достаём оба файла и начинаем анализ бекдора
18. Бинарь представляет собой стрипанный статически слинкованный ELF64 на Си
19. В начале бекдор скрывает свой процесс с помощью модифицированого системного вызова
20. Далее читается файл с конфигом и расшифровываются все записи конфига
21. Каждая запись конфига представляет собой некоторую команду: подключение по сети, команда ОС, шифрование файла
22. При установке сетевого соединения выполняется обратное подключение на заданный адрес в конфиге и запрашивается пароль доступа
23. Пароль проверяется бекдором с помощью модифицированного системного вызова `access`
24. Найдём этот системный вызов в ядре и проанализируем его
25. Сразу в начале кода системного вызова можно найти вызовов функции `kaDILjlf` 
26. Данная функция производит проверку введёного пароля по некоторому несложному алгоритму
27. Смысл алгоритма заключается в нахождении индекса очередной буквы флага в некотором алфавите. Далее на основе этого индекса выбирается матрица из памяти размером 8 на 8 и рассчитывается её определитель, значение определителя используется как seed для LCG, после чего образуется XOR-ключ путём XOR-а 16-раундов LCG друг с другом. Ключ используется для шифрования одного символа флага.
28. Вытаскиваем все данные из ядра и реализуем алгоритм


[Пример нахождения всевозможнных символов на каждой позиции флага](solution/restore_password/main.c)

[Пример восстановления флага на основе знания хэша](solution/restore_password/helper.py)


## Флаг

`CUP{B8ZRAytWjre8AUb0yWFuLJIKAHwlYXys}`