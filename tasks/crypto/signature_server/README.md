# Crypto | Hard | Сервер подписи

## Информация

На специальном сервере компании X можно подписать любое число < 2^100 и узнать точное время вычисления этой подписи. Сможете ли вы получить закрытый ключ?

## Деплой

cd deploy && docker-compose up --build -d

## Выдать участникам

Исходный код сервера [server_task.py](public/server_task.py)

## Описание

Атака Minerva на ECDSA. Сервер подписывает сообщения участников, прикрепляя время подписи (зависящее от длины эфемерного ключа). 
Задача участников - по метке времени выбрать подписи на коротких эфемерных ключах и осуществить атаку методом приведения базиса решетки

## Решение

Решение представлено в файле [writeup.py](solution/writeup.py)

## Флаг

CUP{Minerva_The_curse_0f_ECDSA_n0nc3s}