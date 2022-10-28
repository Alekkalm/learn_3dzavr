# Материалы для курса vectozavr.ru

Сам курс: [vectozavr.ru](https://vectozavr.ru)

## Задания

Задания находятся в файлах lesson(n).cpp

Вам необходимо реализовать все места 3D движка, которые помечены меткой TODO.

<h4>Установка и запуск:</h4>

1) Скачайте и установите библиотеку [OpenAL](https://openal.org/downloads/), которая необходима для поддержки звуков в SFML (без [OpenAL](https://openal.org/downloads/) движок не запустится).


2) Установите [SFML](https://www.sfml-dev.org/download.php) в вашу систему (<b>Версия компилятора должна совпадать на 100%</b>).


2) Выберете подходящий редактор кода. Например, [CLion](https://www.jetbrains.com/clion/) или [Visual Studio](https://visualstudio.microsoft.com/ru/).
     

3) Клинируйте этот репозиторий.
    

4) Соберите проект, добавив в цель сборки один из файлов lesson(n).cpp

Если у вас возникли какие-либо трудности в работе с репозиторием или вы нашли ошибку, то лучше всего будет создать [issue](https://docs.github.com/en/issues/tracking-your-work-with-issues/creating-an-issue).

Общая структура проекта:
![Project demonstration](img/structure.png)




### Как делаю я для винды без clion:

#### Установил все по инструкции автора:
* утилита CMake
* библиотека SFML для windows и компилятора MinGW - положил в папку C:\Libraries,
* компилятор MinGW,
* библиотека OpenAL.


#### В папке проекта создал подпапку "build".<br>
Туда будет CMake генерировать свои файлы.

#### Генерируем Makefile файл:
Запускаю CMake (cmake-gui).<br>
В строке исходников - указываю папку нашего проекта.<br>
В строке "Where to build the binaries:" указываю нашу папку "build".<br>
Нажимаю кнопку "Configure". <br>
     появляется окно в котором нужно выбрать наш геренатор "MinGW MakeFiles" - (мы указываем для какого инструмента нужно сгенерировать данные)<br>
      и в качестве компилятора выбираем опцию "Use default native compilers",<br>
      жмем "Finish".<br>
     CMake пишет какой компилятор он нашел, и так же пишет что нашел библиотеку SFML. <br>
     В окне CMake появляются строки выделеные красным - похоже это параметры которые мы можем поменять. Пока оставим так как есть. В хелпе CMake написано что      надо нажимать "Configure" пока строчки не станут белыми. То есть он находит переменные, и показывает их нам красным цветом чтобы мы их проверили.<br>
Нажимаю кнопку "Generate". <br>
     (CMake пишет "Generating done", а в папке "build" появляется файл "Makefile".)<br>

#### Компилируем проект:
Данные для компиляции подготовлены (файл "Makefile" - для MinGW), теперь можно компилировать.<br>
запускаем консольное окно командной строки (cmd).<br>
переходим в папку нашего проекта. <br>
запускаем файл "mingw32-make.exe" - (пишем в командной строке `mingw32-make.exe`)<br>
Начинается компиляция проекта, и в папке нашего проекта появляется файл "learn_3dzavr.exe".<br>

#### Еще один вариант генерации Makefile:
Сгенерировать Makefile можно и без графического интерфейса CMake:<br>
нужно в консольном окне командной строки перейти в папку "build" и написать команду <br>
`cmake -G "MinGW Makefiles" ..`<br>

#### Еще один вариант компиляции:
Компиляцию тоже можно запустить из CMake из командной строки:<br>
находясь в консольном окне командной строки в папке "build" нужно написать команду<br>
`cmake --build .`<br>

#### О командах CMake:
Подробнее о вариантах команд CMake можно узнать написав команду <br>
`cmake --help`<br>


#### дополнение по возможным IDE
здесь рассказывается как настроить CodeBlocks для работы с библиотекой SFML:<br>
https://www.sfml-dev.org/tutorials/2.5/start-cb.php

Оказывается для CodeBlocks нужна своя версия SFML, для VisualStudio - своя версия SFML, для MinGW - своя версия:<br>
https://www.sfml-dev.org/download/sfml/2.5.1/

поэтому пока CodeBlocks и VisualStudio как IDE не рассматриваем.

#### CodeLite в качестве IDE (редактор кода и дебагер)
Пока работает следующим образом:<br>
В графическом интерфейсе CMake когда нажимаю Configure - выбираю "CodeLite MinGW Makefiles" <br>
 - появляются переменные которые нужно проверить и поменять если надо (выделеные красным). В переменной CMAKE_BUILD_TYPE пишу "Debug", и опять нажимаю Configure. Затем Generate.
В папке buid появляется файл learn_3dzavr.workspace и learn_3dzavr.project - файлы для программы Codelite.
 
В командной строке, как указано выше, из папки build запускаю 
`cmake --build .`<br>
В папке learn_3dzavr появляется learn_3dzavr.exe <br>

Файлы learn_3dzavr.workspace и learn_3dzavr.project - нам нужны чтобы открыть наши файлы как проект в рабочем пространстве.<br>
Файл learn_3dzavr.exe нам нужен чтобы можно было дебажить.

Запускаем CodeLite, открываем WorkSpace (указываем наш файл learn_3dzavr.workspace).<br>

в меню Debugger -> QuickDebug.. <br>
в поле  Debuggee: Exequtable указываем на наш файл learn_3dzavr.exe. <br>


Здесь пишут как настроить SFML для CodeLite <br>
https://en.sfml-dev.org/forums/index.php?topic=18820.0 <br>
и здесь тоже:
https://stackoverflow.com/questions/64629891/how-to-configure-codelite-ide-to-use-c-and-sfml-library-windows-linux-confi <br>

оказалось что мне было достаточно на молоток (Build Active Project) и ошибки про то что он невидит SFML библиотеку пропали. (То есть не пришлось настраивать все как написано по ссылке).

вот здесь пишут какие IDE имеют интеграцию с CMake нативно: <br>
file:///C:/Program%20Files/CMake/doc/cmake/html/guide/ide-integration/index.html <br>

#### IDE KDevelop
Опенсорсная бесплатная IDE с интегрированным CMake.<br>
В настройках можно выбрать: "Настройки" -> "Настроить KDevelop.." -> CMake -> DefaultGenerator: -> MinGWMakefiles.<br>
Затем открыть проект: "Проект" -> "Открыть/импортировать проект.." -> выбрать нашу папку.<br>
Слева сбоку вкладка "Файловая система" - это проводник. Я не сразу распознал. Только через час наверно. Присмотритесь. ) <br>
Отладку пока не запустил, ругается что не может найти LLDB дебагер. <br>

#### VSCode
здесь описание для Linux, но оно подходит и для Windows <br>
https://code.visualstudio.com/docs/cpp/CMake-linux <br>
У меня ругался что не может найти генератор.<br>
Поэтому я в параметрах Cmake tool плагина в переменную CMake:Generator ввел текс "MinGW Makefiles"<br>
И потом нажал кнопку Build внизу в статусной строке.<br>
И увидел что Cmake смог собрать проект.<br>

Быстрый старт:<br>
Создать папку.<br>
перейти в нее из командной строки.<br>
в командной строке написать `code .`<br>
(запустится vscode и будет считать текущую папку WorkSpace-ом.)<br>

создание проекта HelloWorld<br>
открыть Command Palette (нажать Ctrl+Shift+P), вписать команду "CMake: Quick Start"<br>
vscode предложит выбрать kit (kit - в данном случает это набор инструментов (по английски это звучит как toolchain) в который входят compiler, linker, и др.)<br>
я выбираю GCC...<br>
затем - предложит ввести имя проекта.<br>
затем - выбрать тип проекта Executable - для exe файла, или Library для dll. <br>
Создастся CMakeLists.txt и main.cpp<br>

документация по CMake tools для vscode:<br>
https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md <br>
что то про дебагер vscode <br>
https://code.visualstudio.com/docs/cpp/config-wsl <br>

#### Как запустился проект в VSCode
После того как я в параметрах Cmake tool плагина в переменную CMake:Generator ввел текс "MinGW Makefiles"<br>
затем закрыл vscode.<br>
В командной строке перешел в папку нашего проекта learn_3dzavr,<br>
и в командной строке ввел `code .` <br>
VSCode открыла наш проект, и когда я перешел в окно редактированя файла test_scene.cpp там были выделены ошибки про то что не может найти библиотеку SFML.<br>
Я нажал Build внизу в статусной строке, и когда сборка проекта закончилась то ошибок больше небыло.<br>
И когда нажал на божью коровку внизу в статусной строке, то запустился дебагер.<br>
То есть все работает.<br>


