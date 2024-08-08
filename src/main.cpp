#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>

void splitLine(std::vector<std::string> &vec, const std::string line, const char delimeters) {
    int j = 0;
    while (j < line.size()) {
        std::string sub_str = "";
        int i = j;
        while ((line[i] != delimeters) && (i < line.size())) {
            sub_str += line[i];
            i++;
        }
        vec.push_back(sub_str);
        j=i+1;
    }
}

void add(int argc, std::vector<std::string> argv);

int main() {
    setlocale(LC_ALL, "RU");

    std::string command = "";
    int argc = 0;
    std::vector<std::string> argv{};

    while (true) {
        std::cout << "Введите команду: ";
        std::getline(std::cin, command);
        splitLine(argv, command, ' ');
        argc = argv.size();

        std::map < std::string, std::function<void(int argc, std::vector<std::string> argv)>> map = {
        {"add", add}
        };

        if (map.find(argv[0]) != map.end()) {
            map[argv[0]](argc, argv);
        }
        else {
            std::cout << "Команда не найдена\nВведите help для помощи\n" << std::endl;
        }


        argc = 0;
        argv.clear();
    }
    





 /*   time_t seconds = time(NULL);
    char time_line[31];
    tm* timeinfo = localtime(&seconds);

    
    strftime(time_line, 31, "Сейчас %d.%m.%Y Время: %H:%M", timeinfo);
    std::cout << time_line << std::endl;

    std::cout << "Сейчас: " << timeinfo->tm_mday << "." << timeinfo->tm_mon + 1 << "." << timeinfo->tm_year + 1900 << " Время: " << timeinfo->tm_hour << ":" << timeinfo->tm_min << std::endl;
    //std::cout << time_line << std::endl << difftime(10, 20) << std::endl;*/


    system("pause");
    return 0;
}


void add(int argc, std::vector<std::string> argv) {
    std::cout << "add1" << std::endl;
}