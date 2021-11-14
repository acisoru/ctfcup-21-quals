# Pwn | easy | Mnogoetazhka v1

## Информация

> Построй свою многоэтажку!
>
> `nc <ip> 17102`
>

## Деплой

```sh
cd deploy
docker-compose up --build -d
```

## Выдать участинкам

Архив из директории [public/](public/) и IP:PORT сервера

## Описание

ELF64, C++

## Решение

1. unsorted bin leak для получения утечки адреса
2. tcache poisoning для перезаписи `__malloc_hook` на `one_gadget`

[Эксплоит](solution/sploit.py)

## Флаг

`CUP{903ee31fcc0c9f5a05d124eea99fc280}`