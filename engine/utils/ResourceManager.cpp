//
// Created by Neirokan on 09.05.2020
//

#include <map>
#include <memory>
#include <sstream>
#include <fstream>

#include "ResourceManager.h"
#include "Log.h"

ResourceManager *ResourceManager::_instance = nullptr;

void ResourceManager::init() {
    delete _instance;
    _instance = new ResourceManager();

    Log::log("ResourceManager::init(): resource manager was initialized");
}

std::shared_ptr<sf::Texture> ResourceManager::loadTexture(const std::string &filename) {
    if (_instance == nullptr) {
        return nullptr;
    }

    // If texture is already loaded - return pointer to it
    auto it = _instance->_textures.find(filename);
    if (it != _instance->_textures.end()) {
        return it->second;
    }

    // Otherwise - try to load it. If failure - return zero
    std::shared_ptr<sf::Texture> texture(new sf::Texture);
    if (!texture->loadFromFile(filename)) {
        Log::log("ResourceManager::loadTexture: error with loading texture '" + filename + "'");
        return nullptr;
    }

    Log::log("ResourceManager::loadTexture: texture '" + filename + "' was loaded");

    // If success - remember and return texture pointer
    texture->setRepeated(true);
    _instance->_textures.emplace(filename, texture);

    return texture;
}

std::shared_ptr<sf::SoundBuffer> ResourceManager::loadSoundBuffer(const std::string &filename) {
    if (_instance == nullptr) {
        return nullptr;
    }

    // If sound buffer is already loaded - return pointer to it
    auto it = _instance->_soundBuffers.find(filename);
    if (it != _instance->_soundBuffers.end()) {
        return it->second;
    }

    // Otherwise - try to load it. If failure - return zero
    std::shared_ptr<sf::SoundBuffer> soundBuffer(new sf::SoundBuffer);
    if (!soundBuffer->loadFromFile(filename)) {
        Log::log("ResourceManager::loadSoundBuffer: error with loading sound buffer '" + filename + "'");
        return nullptr;
    }

    Log::log("ResourceManager::loadSoundBuffer: sound buffer '" + filename + "' was loaded");

    // If success - remember and return sound pointer
    _instance->_soundBuffers.emplace(filename, soundBuffer);

    return soundBuffer;
}

std::shared_ptr<sf::Font> ResourceManager::loadFont(const std::string &filename) {
    if (_instance == nullptr) {
        return nullptr;
    }

    // If font is already loaded - return pointer to it
    auto it = _instance->_fonts.find(filename);
    if (it != _instance->_fonts.end()) {
        return it->second;
    }

    // Otherwise - try to load it. If failure - return zero
    std::shared_ptr<sf::Font> font(new sf::Font);
    if (!font->loadFromFile(filename)) {
        Log::log("ResourceManager::loadFont: error with loading font: '" + filename + "'");
        return nullptr;
    }

    Log::log("ResourceManager::loadFont: font '" + filename + "' was loaded");

    // If success - remember and return font pointer
    _instance->_fonts.emplace(filename, font);

    return font;
}

std::vector<std::shared_ptr<Mesh>> ResourceManager::loadObjects(const std::string &filename) {

    std::vector<std::shared_ptr<Mesh>> objects{};
    std::map<std::string, sf::Color> maters{};

    if (_instance == nullptr) {
        return objects;
    }

    // If objects is already loaded - return pointer to it
    auto it = _instance->_objects.find(filename);
    if (it != _instance->_objects.end()) {
        return it->second;
    }

    //АК комент:
    //класс Ifstream - считывает из файла и помещает в поток. (чтение файла) 
    //класс ofstream - берутся данные из потока и записываются в файл. (запись файла) 
    //https://learn.microsoft.com/ru-ru/cpp/standard-library/basic-ifstream-class?view=msvc-170
    std::ifstream file(filename);//АК комент: это вызывается конструктор класса ifstream. создаем объект типа ifstream с именем file.
    if (!file.is_open()) {
        Log::log("Mesh::LoadObjects(): cannot load file from '" + filename + "'");
        return objects;
    }

    std::vector<Vec4D> verts{};
    std::vector<Triangle> tris{};
    sf::Color currentColor = sf::Color(255, 245, 194, 255);

    while (!file.eof()) {
        char line[128];
        file.getline(line, 128);

        //АК: операторы << и >> 
        //оператор "<<" - запись в поток
        //оператор ">>" - чтение из потока до символа пробела или переноса строки. (символы пробела и переноса строки отбрасываются)
        //для считывания строки целиком с пробелами до переноса строки - использовать функцию getline(); (символ переноса строки отбрасывается)
        //https://metanit.com/cpp/tutorial/8.1.php
        //https://stackoverflow.com/questions/13993951/stringstream-operator
        //https://ejudge.179.ru/tasks/cpp/total/161.html
        std::stringstream s;
        s << line;//АК комент: текст из строки line заносим в строковый-поток  s.

        char junk; //АК комент: переводится как хлам, мусор. Переменная для хранения хлама.

        //АК комент: если первый символ "m" - значив в этой строке описывается materials. (цвет)
        //похоже что в стандарте .obj нет такого кода ("m"), т.к. материал там хранится в отдельном рядом лежащем файле .mtl
        //а ссылка на него прописывается в стреке "mtllib ххх.mtl".
        //какой материал использовать - указывается в строке "usemtl MaterialName"
        //похоже что наша "m" - это дополнение(упрощение) от vectozavr.
        //https://ru.wikipedia.org/wiki/Obj
        //https://www.martinreddy.net/gfx/3d/OBJ.spec
        if (line[0] == 'm') {
            // TODO: implement (lesson 3)
            int color[4];
            std::string matName;
            s >> junk >> matName >> color[0] >> color[1] >> color[2] >> color[3];
            maters.insert({matName, sf::Color(color[0],color[1],color[2],color[3])});
        }
        //АК комент: "v" - вершина (vertex).
        if (line[0] == 'v') {
            // TODO: implement (lesson 3)
            double x, y, z;
            //похоже что stringstream s делает автоматически преобразование типов:
            //мы кусочки строки (слова) помещаем в переменные типа double. (преобразуем string в double)
            //Сами они там не поместятся, их нужно преобразовывать.
            //(хотябы по тому что одно слово -1.000000 занимает 9 байт, а double - 8 байт)
            s >> junk >> x >> y >> z;
            verts.emplace_back(x,y, z, 1.0);
        }
        //АК комент: "f" - поверхность, грань (face)
        if (line[0] == 'f') {
            // TODO: implement (lesson 3)
            int v[3]; //номер вершины, (считываем из строки такого вида: "f x x x")
            s >> junk >> v[0] >> v[1] >> v[2];
            //т.к. в строке "f x x x" используется нумерация начинающаяся с 1,
            //а у нас массивы используют нумерацию начинающуюся с 0,
            //то нужно ввести поправку (сделать цифры которые мы считали из строки "f x x x" - на 1 меньше.)
            tris.emplace_back(verts[v[0]-1], verts[v[1]-1], verts[v[2]-1], currentColor);

        }
        //АК комент: группа. точно непонятно для чего она. 
        //мы используем для получения материала. (сами так придумали и поправили.(добавили в конец имени группы 3 символа с номером материала))
        //поэтому будем считывать 3 последних символа.
        if (line[0] == 'g') {
            // TODO: implement (lesson 3)
            std::string matInfo;
            s >> junk >> matInfo;

            std::string colorName = matInfo.substr(matInfo.size()-3, 3);
            //задаем текущий цвет.
            //в файле .obj - имя группы идет раньше треугольников (faces), поэтому сначала задастся цвет,
            //а потом будут добавлятся треугольники этого цвета.
            currentColor = maters[colorName];
        }
        //АК: "o" - новый объект
        if (line[0] == 'o') {
            // TODO: implement (lesson 3)
            //в файле .obj - объект объявляется перед описанием точек и треугольников
            //но мы будем просто по признаку что начинается описание нового объекта в файле .obj
            //из предыдущих уже созданных треугольников формировать объект с именем "filename_temp_obj_xxx"
            //закидывать его в коллекцию, и удалять треугольники из которых мы его создавали.
            if(!tris.empty()){
                objects.emplace_back(std::make_shared<Mesh>(ObjectNameTag(filename + "_temp_obj_" + std::to_string(objects.size())), tris));
                tris.clear();
            }
        }
    }

    //АК комент: а здесь мы заталкиваем в коллекцию объектов - последний объект, который мы создадим.
    //т.к. больше обхектов в файле .obj объявлятся не будет, а из последних полученых треугольников нам надо создать последний объект
    if (!tris.empty()) {
        objects.push_back(std::make_shared<Mesh>(ObjectNameTag(filename + "_temp_obj_" + std::to_string(objects.size())), tris));
    }
    tris.clear();
    file.close();

    Log::log("Mesh::LoadObjects(): obj '" + filename + "' was loaded");

    // If success - remember and return vector of objects pointer
    _instance->_objects.emplace(filename, objects);

    return objects;
}

void ResourceManager::unloadTextures() {
    if (_instance == nullptr) {
        return;
    }

    int texturesCounter = _instance->_textures.size();
    for (auto &_texture : _instance->_textures) {
        _texture.second.reset();
    }
    _instance->_textures.clear();

    Log::log("ResourceManager::unloadTextures(): all " + std::to_string(texturesCounter) + " textures was unloaded");
}

void ResourceManager::unloadSoundBuffers() {
    if (_instance == nullptr) {
        return;
    }

    int soundBuffersCounter = _instance->_soundBuffers.size();
    for (auto &_soundBuffer : _instance->_soundBuffers) {
        _soundBuffer.second.reset();
    }
    _instance->_soundBuffers.clear();

    Log::log("ResourceManager::unloadSoundBuffers(): all " + std::to_string(soundBuffersCounter) +
             " soundBuffers was unloaded");
}

void ResourceManager::unloadFonts() {
    if (_instance == nullptr) {
        return;
    }

    int fontsCounter = _instance->_fonts.size();
    for (auto &_font : _instance->_fonts) {
        _font.second.reset();
    }
    _instance->_fonts.clear();

    Log::log("ResourceManager::unloadFonts(): all " + std::to_string(fontsCounter) + " fonts was unloaded");
}

void ResourceManager::unloadObjects() {
    if (_instance == nullptr) {
        return;
    }

    int objCounter = _instance->_objects.size();
    _instance->_objects.clear();

    Log::log("ResourceManager::unloadObjects(): all " + std::to_string(objCounter) + " objects was unloaded");
}

void ResourceManager::unloadAllResources() {
    unloadTextures();
    unloadSoundBuffers();
    unloadFonts();
    unloadObjects();

    Log::log("ResourceManager::unloadAllResources(): all resources was unloaded");
}

void ResourceManager::free() {
    unloadAllResources();

    delete _instance;
    _instance = nullptr;

    Log::log("ResourceManager::free(): pointer to 'ResourceManager' was freed");
}
