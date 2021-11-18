# Crypto | Easy | x ^= y; y ^= x; x ^= y

## Информация

В нашем замечательном лицее преподают довольно прогрессивные предметы. Например, основы криптографии!

И для выставки наши ученики сделали пару тасков, вот один из них, совсем не сложный.

## Деплой

Не требуется

## Выдать участникам

Файл с системой ключей [keys.txt](files/keys.txt)
Файл с зашифрованным флагом [flag.txt](files/flag.txt)

## Описание

Baby crypto warmup

## Решение

В названии таска дана подсказка - классическая задача по обмену значений в переменных, используя свойства функции xor. Что, по идее, должно подтолкнуть к вспоминанию свойств xor-а.

Мною предполагалось что участники пойдут двумя путями:

### Решение, где не надо думать

Просто берем и просчитываем все ключи. Получим

```
k1 = dfc33ec22e40ce24e82bc4ac1f5aae42a43ddf3dd708cbda5fa6
k2 = a105106200ed904a1bac26e3c2bc94dd95234440a32df61ca4ae
k3 = 7ec62ea02ead5e6ef387e24fdde63a9f311e9b7d74253dc6fb08
k4 = 2bdeccba2e30a8ecefacfa4e2cd2afdf7fdc6afcd6571a22d9c1
k5 = 5eb909225cae6193e4b3e27c3930ebe36b1ab3bb4b23861152ae
k6 = 7ec62ea02ead5e6ef387e24fdde63a9f311e9b7d74253dc6fb08
k7 = 5eb909225cae6193e4b3e27c3930ebe36b1ab3bb4b23861152ae
k8 = f41df278007066c807873ee23388019ddbe1b5c1015fd1f88667
k9 = f41df278007066c807873ee23388019ddbe1b5c1015fd1f88667
k10 = 3d1fac0ed24ca9f2f71edecaafd4e5d833ad382f71ed8e4aa5f8
k11 = 2bdeccba2e30a8ecefacfa4e2cd2afdf7fdc6afcd6571a22d9c1
k12 = aaa4fb5a5cde075be334dc9e0ab8ea7eb0fb067a4a7c57e9d4c9
```
И ксорим все ключи с выданным шифротекстом в flag.txt... Но уже тут можно увидеть повторяющиемся ключи и подумать о простом решении

### Простое решение, где надо немного подумать

Особенностью булевой функции xor является то, что если ее наложить два раза подряд ключ на данные, то он исключит сам себя. И соответственно, открытый текст останется открытым. Данная особенность является очевидной и предлагается доказать это свойство самостоятельно (с) Лектор по алгебре.

В данном таске можно заметить особенность, что несколько ключей получаются одинаковыми, если выписать несколько ключей руками. 

Соответственно, учитывая эту особенность, решение состоит из трех ксоров: $flag \oplus k3 \oplus k10 \oplus k12$.

[Ссылка на шефа с решением](https://gchq.github.io/CyberChef/#recipe=From_Hex('Auto')XOR(%7B'option':'Hex','string':'7ec62ea02ead5e6ef387e24fdde63a9f311e9b7d74253dc6fb08'%7D,'Standard',false)XOR(%7B'option':'Hex','string':'3d1fac0ed24ca9f2f71edecaafd4e5d833ad382f71ed8e4aa5f8'%7D,'Standard',false)XOR(%7B'option':'Hex','string':'aaa4fb5a5cde075be334dc9e0ab8ea7eb0fb067a4a7c57e9d4c9'%7D,'Standard',false)To_Hex('None',0)From_Hex('None')&input=YWEyODI5OGZjMzRkODliNzkzYzJiZjcxMGRmOTQxNjZkNDI3ZDc3NzM4ZDU5NjA4ZmY0OTk0)

## Флаг

CUP{crypto_just_for_warmup}