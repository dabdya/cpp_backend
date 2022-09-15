
### Необходимые библиотеки
```sudo apt update && sudo apt-get install libcpprest-dev libpq-dev libfmt-dev```

### Сборка
В директории ```src``` нужно набрать 
```g++ -o server $(find . -name "*.cpp") --std=c++17 -I /usr/include/postgresql/ -Iheaders/ -lcpprest -pthread -lcrypto -lfmt -lpq```. 
Через утилиту ```cmake``` собрать не получилось, поэтому такой позор.
