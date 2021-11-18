# Crypto | Medium | Операция "Фактор"

## Информация

ОТ: Лейтенант П. Х.
Тема: Работа по специальности.

Понаучат детей криптографии блин. Слышал, что в нашем лицее крипте учат?
Так вот, на выставке у одного из школьников это новомодное изобретение было - передача данных со сквозным, блин, шифрованием.

И что ты думаешь? Они каким-то образом умудряются проносить при помощи этого шпоры на уроки информатики. И их учительница все рассказала нашему Майору! Он сразу взорвался, начал кричать "Чтобы ключи шифрования были у меня на столе до обеда!". Видно что-то смыслит в этой вашей криптографии.

Короче, все материалы дела приложу к сообщению в архивчике. У тебя есть 24 часа.

## Деплой

Не требуется

## Выдать участникам

Все необходимые файлы в архиве [files.rar](files/files.rar)

## Описание
Elliptic Cryptography

## Решение

Название таска и инициалы отправителя намекают на атаку Поллинга-Хеллмана, через факторизацию порядка точки-генератора.

Так как параметры кривой не особо большие, то основной сложностью является написать реализацию атаки. В теории можно было просто перебрать все значения ¯\\\_(ツ)_/¯.

Реализация ниже, источник алгоритма: `Guide to Elliptic Curve Cryptography; D.Hankerson, A.Menezes, S.Vanstone; Глава 4.1.1;`

```python
# Часть 1 - получения секретной компоненты.
F.<a> = GF(7566955095017822821)
E = EllipticCurve(F, [1, 1910])

print(E)
P = E(0x658e022bff8196da, 0x3f4d80f2049ce846)

print(P)
print("P.order =",P.order())
print("P.is_prime =", P.order().is_prime())

Q = E(0x6079aa84b3ac254f, 0x0f8339237d2213cb)
B = E(0x0b0f2f1142eedbf1, 0x2acb47b0373a56ca) # для получения ключа

# по оригинальной формуле - n = p1 ^ e1 * ... * p_n ^ e_n
factors = [ [2,3], [3,1], [11,2], [31,1], [37,1], [79,1], [9585456059,1]]
crt_arr_reminders = [] 
crt_arr_modulas = [ p[0] ** p[1] for p in factors]

for co_factor in factors:
    z = [0] # массив наших дискретных логарифмов
    P_0 = int(P.order() / (co_factor[0])) * P
    for e in IntegerRange(1, co_factor[1] + 1 ) :
        Q_multi = Q
        for i in range(e):
            Q_multi = Q_multi - int(z[i]*( co_factor[0] ** (i-1)))*P
        Q_i = int(P.order() / (co_factor[0]**e)) * Q_multi 
        print(Q_i, P_0)
        z.append(  discrete_log_rho(Q_i,P_0, operation='+') )
    print(z)
    res_for_crt = 0
    for i in range(1, len(z)):
        res_for_crt += z[i]*(co_factor[0] ** (i-1))
    print("New reminder is", res_for_crt)
    crt_arr_reminders.append(res_for_crt)

res =  crt(crt_arr_reminders, crt_arr_modulas)
print("Answer is", res)
print("Check Answer is", res*P == Q)
key = res*B
print(key)

# Часть 2 - расшифрование сообщения
text = b"\x7a\xbc\x25\xba\x38\x39\xd0\xe8\x5a\xf3\x38\xa6\x38\x38\xd2\xe3\x09\xb6\x26\xbc\x23\x34\xd4\xe3\x4e\xa0\x68\xb2\x2d\x3b\xd8\xad\x44\xb6\x68\xbc\x3e\x29\x93\xa3\x07\xf3\x07\xb7\x60\x70\xdb\xe1\x48\xb4\x69\xff\x0f\x05\xed\xf6\x1a\xbf\x24\xee\x3c\x22\x8c\xee\x76\xb0\x3d\xad\x3a\x63\xe2\xfd\x1d\xa1\x7c\xb2\x3f\x0f\xd0\xf8\x5a\xa7\x17\xbd\x7f\x0f\xce\xbe\x4a\xa6\x3a\xec\x31"

import base64
key = (int(key.xy()[0]) + int(key.xy()[1]))  # из protocol.py
key = key.to_bytes(8, byteorder='big')
print(base64.b64encode(key))
res = ''
print(key)
res = ''
for c in range(len(text)):
    res += chr(text[c] ^^ key[c % len(key)])
print(res)
```

## Флаг

`CUP{3ll1pr1c_curv3_p4r4ms_must_b3_s3cur3}`

P.s. Ошибка сделана специально, вдруг кто перебрать пытался :D