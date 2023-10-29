<details open>
<summary><span style="color:tomato;font-size:16px">THREADS</span></summary>
<details open>
<summary><span style="color:tomato;font-size:12px">Заплыв на 100 метров</span></summary>

Реализуйте симуляцию состязаний по заплыву на 100 метров.

Всего шесть дорожек и шесть пловцов. 
Имена пловцов указываются вначале из стандартного ввода. 
Каждый из них плывёт с разной скоростью, которая также задаётся пользователем при старте, 
через стандартный ввод в метрах в секунду.

Каждую секунду в стандартный вывод выдаётся информация о том, сколько проплыл тот или иной пловец.

Как только все пловцы коснулись 100-метровой отметки, 
заплыв заканчивается и выводится таблица с итоговыми результатами, 
отсортированная по возрастанию итогового времени заплыва.

</details>

<details open>
<summary><span style="color:tomato;font-size:12px">Выполнение программы:</span></summary>

<details>
<summary><span style="color:sienna;font-size:12px">Общая информация:</span></summary>

Скорость пловцов измеряется метрами в секунду. 
Поэтому заносится в тип double и находится в интервале 2 - 3. 
В реальности - это медианные значения для опытных пловцов на дальность 100м.

</details>

<details open>
<summary><span style="color:sienna;font-size:12px">Основа:</span></summary>

Для демонстрации многопоточности, создаётся отдельный поток `std::thread threadMain`.
Внутри него (функция `doSwim`), создаётся ещё ряд дочерних потоков: 
для каждого нового объекта (в нашем случае - спортсмена). 

Сначала они собираются в единый вектор `threads`. 
Затем каждый из потоков вызывает функцию `asyncCountdown`.

```c++
void doSwim(vector<Swimmer*> swimmers, double distance, int swimmersCount) {
    vector<std::thread> threads(swimmersCount);

    for (int i = 0; i < swimmersCount; ++i) {
        threads.emplace_back(asyncCountdown, swimmers[i], distance);
    }

    while (finisherCounter < swimmersCount) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Если все спортсмены достигли финиша:
    isFinish = true;
    
    for (auto &thread : threads) { if (thread.joinable()) { thread.join(); } }
}
```

На что нужно обратить внимание:

- Создаётся несколько параллельных потоков. Кстати, привязать callback к каждому из них можно и
альтернативным равнозначным способом:

```c++
// 1 вариант:
threads.emplace_back(asyncCountdown, swimmers[i], distance);
// 2 вариант:
threads.emplace_back([&swimmers, distance, i]() { asyncCountdown(swimmers[i], distance); });
```

- Параллельно запускается и основной процесс. Т.е. цикл `while (finisherCounter < swimmersCount)`.  
Важно то, что он выполняется параллельно с вышеобозначенными потоками. 
И важно то, что в параллельных потоках переменная `finisherCounter` будет меняться гарантированно увеличиваясь. 
Это позволит, в итоге, завершить цикл и поменять переменную `isFinish`, 
которая остановит и параллельные основному потоки:

```c++
while (finisherCounter < swimmersCount) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

Далее. В каждом из параллельных потоков выполняется `asyncCountdown`:

```c++
void asyncCountdown(Swimmer* swimmer, double distance) {
    int currentSecond = 0;

    while (!isFinish) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        watchSwimmerList.lock();
        double position = swimmer->getSpeed() * currentSecond;

        // Если достигнут финиш, то:
        if (position >= distance) {
            swimmer->setTimeSpent(distance / swimmer->getSpeed());            
            ++finisherCounter;
        }
        watchSwimmerList.unlock();
        
        if (swimmer->hasFinish() > 0) { return; }

        ++currentSecond;
    }
}
```

Здесь условием работы цикла мог быть и простой true.
При этом выход из него обеспечивался всё тем же условием `if (swimmer->hasFinish() > 0) { return; }`.

Однако в программе хотелось показать, 
что цикл здесь выполняется до тех пор, пока не поменяется, в том числе, переменная `isFinish`. 
Это произойдет лишь тогда, когда в каждом (!!!) из циклов спортсмен достигнет финиша и
тогда изменится переменная `finisherCounter`. 

Тогда в основном потоке прекратится выполнение цикла `while (finisherCounter < swimmersCount)`, 
что, в свою очередь, приведёт к `isFinish = true`.

Чтобы заполнение/сортировка/удаление общего списка пловцов было корректным, используется mutex `watchSwimmerList`.

В финале, после распечатывания отчета, все объекты кучи - удаляются.

</details>

</details>

<details>
<summary><span style="color:tomato;font-size:12px">Полезные ссылки</span></summary>

<p><a href="https://thispointer.com/c11-how-to-create-vector-of-thread-objects/" style="margin-left:16px">How to create Vector of Thread Objects</a></p>

<p><a href="https://stackoverflow.com/questions/30768216/c-stdvector-of-independent-stdthreads" style="margin-left:16px">std::vector of independent std::threads</a></p>

<p><a href="https://stackoverflow.com/questions/32350909/taking-input-over-standard-i-o-in-multithreaded-application" style="margin-left:16px">Taking input over standard I/O in multithreaded application</a></p>

</details>

</details>