# Web | Easy | Personal Info Site

## Информация

> Департамент технологий нашего города представил новый прекраный сервис, который позволяет делиться своей персональной информацией.
>
> Что интересно, на платформе есть вполне живой админ, которому можно пожаловаться на чужую персональную информацию!
>
> Интересно, а что сам админ хранит на платформе ?

## Деплой

Просто деплой.
```sh
cd deploy
docker-compose up --build -d
```

## Выдать участинкам

IP:PORT сервера.

Исходники можно выдать как хинт, если не будет FB за час-два.

## Описание

Простой сервис с XSS.

Нужно обойти CSP с nonce, используя base-tag injection.

## Решение

1. Найти, что в поле "title" есть инъекция html.
2. Вставить base-tag на свой домен.
3. На своем домене поднять static/js/bootstrap.min.js с своим кодом.

[Эксплоит](solution/solve.py)

## Флаг

`CUP{ee34f27f0ddccb3d715eab777fa66592a00b13b3}`

