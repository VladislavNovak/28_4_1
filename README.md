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
<summary><span style="color:tomato;font-size:12px">Пояснения</span></summary>

Скорость пловцов заносится в тип double и находится в интервале между 2 - 3 метрами в секунду. 
Это медианные значения для опытных пловцов на дальность 100м.

Для демонстрации многопоточности, создаётся отдельный поток. 

При его выполнении (функция `doSwim`), создаётся ещё ряд дочерних потоков: 
для каждого нового объекта (в нашем случае - спортсмена). 
Сначала они собираются в единый вектор `threads`, 
каждый поток которого вызывает функцию `doSwimLine`:

```c++
void doSwim(vector<Swimmer*> swimmers, double distance, int swimmersCount) {
    vector<std::thread> threads(swimmersCount);

    for (int i = 0; i < swimmersCount; ++i) {
        // Альтернативные способы создания массива потоков:
        threads.emplace_back(doSwimLine, swimmers[i], distance);
        // threads.emplace_back([&swimmers, distance, i]() { swimmingLane(swimmers[i], distance); });
    }

    for (auto &thread : threads) { if (thread.joinable()) { thread.join(); } }
}
```

Затем - все соседние потоки отрабатывают параллельно. 
Это можно отследить, вызвав в каждой из функций `doSwimLine` код std::this_thread::get_id()

В финале, после распечатывания отчета, все объекты кучи - удаляются.

</details>

<details>
<summary><span style="color:tomato;font-size:12px">Полезные ссылки</span></summary>

<p><a href="https://thispointer.com/c11-how-to-create-vector-of-thread-objects/" style="margin-left:16px"> How to create Vector of Thread Objects</a></p>

<p><a href="https://stackoverflow.com/questions/30768216/c-stdvector-of-independent-stdthreads" style="margin-left:16px">std::vector of independent std::threads</a></p>

</details>

</details>