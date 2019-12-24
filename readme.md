# libasa

Библиотека для управления аналоговым сигнатурным анализатором. Позволяет подключиться к серверно-аппаратной части, управлять настройками оборудования и получать сигнатуры.

Linux:

## Зависимости:
- libcurl
- xmlrpc-c

## Компиляция и использование

Для работы требуется скомпилированный XMLRPC

Путь к корневой папке XMLRPC нужно прописать в файлы:
- Makefile
- runexample.sh

Компиляция библиотеки и примера

```
make
```

Запуск примера

```
chmod 777 ./runexample.sh
./runexample.sh
```

Основной код в файле src/libasa.c.


## Установка зависимостей на Astra Linux

Настройка чистого образа

1. Прописать репозитории apt-get

    ```
    sudo nano /etc/apt/sources.list
    ```

    Файл должен выглядеть так:

    ```
    #deb cdrom:[OS Astra Linux 1.11 orel - amd64 DVD ]/ orel contrib main non-free
    #deb http://mirror.yandex.ru/astra/stable/orel/current/repository orel main contrib non-free
    deb [trusted=yes] http://mirror.yandex.ru/astra/frozen/orel-1.11/repository orel main contrib non-free
    deb [trusted=yes] http://mirror.yandex.ru/astra/frozen/orel-1.11/repository-update orel main contrib non-free
    ```

2. Установить libcurl:

    ```
    sudo apt-get update
    sudo apt-get install libcurl4-openssl-dev
    ```

    Провекра, что всё встало:

    ```
    curl-config --version
    ```

3. Закинуть на машину исходники xmlrpc и собрать их:

    ```
    sudo sh ./configure
    make
    ```

    Проверка:

    ```
    cd examples
    make
    ./xmlrpc_sample_add_server 8080
    ./xmlrpc_sample_add_client (в отдельном терминале)
    ```

Windows:

1. Скачать бинарник Xml-rpc for windows:

    https://sourceforge.net/projects/xmlrpc-c/files/Xmlrpc-c%20Super%20Stable/1.43.08/xmlrpc-c-1.43.08.tgz/download

2. Распаковать и собрать на VS2013 C:\...\xmlrpc-c-1.43.08\Windows\project\vs2010express\vs2010express.sln

3. Собрать и запустить asa.sln

Mingw:
1. Скачать бинарник Xml-rpc for windows:

    https://sourceforge.net/projects/xmlrpc-c/files/Xmlrpc-c%20Super%20Stable/1.43.08/xmlrpc-c-1.43.08.tgz/download

2. Распаковать и собрать на VS2013 C:\...\xmlrpc-c-1.43.08\Windows\project\vs2010express\vs2010express.sln

3. Проверить корректность путей в GNUmakefile и собрать библиотеку:
    make -f GNUmakefile

4. Запустить пример:
    asaexample.exe