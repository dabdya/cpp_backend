
## Как запускать приложение


1. Установить Docker для вашей операционной системы <a href=https://docs.docker.com/desktop/windows/install/>Windows</a> | <a href=https://docs.docker.com/engine/install/>Linux</a>
2. Если работаете под Windows, то нужно дополнительно установить <a href=https://docs.microsoft.com/en-us/windows/wsl/install>WSL</a>, так как контейнеры запускаются на Linux, и нельзя просто так шарить ядро Windows между ними.
3. Склонировать или обновить этот репозиторий.
4. Запустить сервис ```docker``` и из корневой директории проекта вести в терминал команду ```docker-compose build```.
5. Подождать пока сбилдится, в первый раз это может занять некоторое время, при ребилдинге это будет происходить быстрее из-за накопленного кэша.
6. Запустить проект с помощью команды ```docker-compose up```.

## Комментарии к решению

1. При деплое в предоставленный мне контейнер я не успел обернуть приложение в докер, поэтому там ничего не доступно. Однако я уверен, что если бы была возможность еще раз задеплоить, то вышеуказанный алгоритм сработал бы и приложение было бы доступно из глобальной сети.

2. Использовал библиотеку ```cpprest```, валидацию делал долго и мучительно и все равно не до конца правильно, при тестировании некоторые аттрибуты не матчатся: формат даты, вместо ```nulll``` отображается пустая строка, ```size``` отображается как строка.

3. Не доделал фичу когда нужно было рекурсивно посчитать размер директории при добавлении в нее файлов.
