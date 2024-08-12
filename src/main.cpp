#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include <regex>
#include <sstream>


using json = nlohmann::json;


void splitLine(std::vector<std::string> &vec, const std::string line, const char delimeters) {
    int j = 0;
    int count = 0;
    while (j < line.size()) {
        std::string sub_str = "";
        int i = j;
        while ((line[i] != delimeters) && (i < line.size())) {
            sub_str += line[i];
            i++;
            count++;
        }
        if (count > 0) {
            vec.push_back(sub_str);
        }
        count = 0;
        j=i+1;
    }
}
time_t convertStringToTimeT(const std::string date) {
    struct tm tm = { 0 };

    std::vector<std::string> line{};
    splitLine(line, date, '.');

    tm.tm_mday = atoi(line[0].c_str());
    tm.tm_mon = atoi(line[1].c_str()) - 1;
    tm.tm_year = atoi(line[2].c_str()) - 1900;

    time_t timeStamp = mktime(&tm);
    return timeStamp;
}
template<class T>
bool in_range(const T t, T min, T max) {
    return t >= min && t <= max;
}

void help(int argc, std::vector<std::string> argv);
void info(int argc, std::vector<std::string> argv);
void addExpense(int argc, std::vector<std::string> argv);

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
        {"help", help},
        {"info", info},
        {"addExpense", addExpense}
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

    system("pause");
    return 0;
}


void help(int argc, std::vector<std::string> argv) {
    std::cout << "addExpense\tДобавление покупки в планировщик\n" << std::endl;
}

void info(int argc, std::vector<std::string> argv) {
    time_t seconds = time(NULL);
    char time_line[31];
    tm* timeinfo = localtime(&seconds);


    strftime(time_line, 31, "Сейчас %d.%m.%Y Время: %H:%M", timeinfo);
    std::cout << time_line << std::endl;

    json data;
    std::ifstream input("data.json");
    if (input.is_open()) {
        if (!(input.peek() == -1)) {
            input >> data;
        }
        else {
            data = "";
        }
        input.close();
    }
    else {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json\n";
       return;
    }

    if (!data.contains("Expense")) {
        std::cout << "Покупок не запланированно\n";
    }
    else {
        std::cout << "Всего запланированно " << data["Expense"].size() << " покупок:\n";
        for (int i = 0; i < data["Expense"].size(); i++) {
            std::cout << data["Expense"][i]["name"] << " - " << data["Expense"][i]["price"] << " руб\n";
        }
    }


    /*time_t t = convertStringToTimeT("07.07.2005");
    time_t tg = difftime(seconds, t);
    tm* time = gmtime(&tg);
    time->tm_year = time->tm_year - 1970;
    time->tm_mon = time->tm_mon - 1;
    time->tm_mday = time->tm_mday - 1;
    strftime(time_line, 31, "Сейчас %d.%m.%Y Время: %H:%M", time);
    std::cout << time_line << std::endl;*/

}

void addExpense(int argc, std::vector<std::string> argv) {

    if (argc == 1) {
        std::cout << "Добавление покупки в планировщик\n\naddExpense <name> <price> <date> <description>\n\nАргументы\n\t<name>\t\tИмя покупки\n\t<price>\t\tСтоимость покупки. Целое число\n\t<date>\t\tДата планируемого совершения покупки в формате dd.MM.yyyy\n\t<description>\tОписание покупки\n\n";
        return;
    }

    if (argc < 5) {
        std::cerr << "Ошибка!\nНедостаточно аргументов\n\n";
        return;
    }
    for (int i = 5; i < argc; i++) {
        argv[4] += (" " + argv[i]);
    }

    std::regex num("[0-9]*");

    std::ifstream input("data.json");
    json data;

    if (!input.is_open()) {
        data["Expense"] = json::array();
    }
    else {
        if (!(input.peek() == std::ifstream::traits_type::eof())) {
            input >> data;
        }
    }
    input.close();

    for (int i = 0; i < data["Expense"].size(); i++) {
        if (data["Expense"][i]["name"] == argv[1]) {
            std::cout << "Покупка " << argv[1] << " уже существует!\nЕсли вы хотите отредактировать покупку то используйте команду editExpense\n\n";
            return;
        }
    }
    

    json object;

    object["name"] = argv[1];

    if (regex_match(argv[2], num)) {
        object["price"] = atoi(argv[2].c_str());
    }
    else {
        std::cerr << "Ошибка!\nВ поле цены не должны присутствовать символы, кроме цифр\n\n";
        return;
    }

    std::vector<std::string> date_vec{};
    splitLine(date_vec, argv[3], '.');
    if (date_vec.size() == 3 &&
        (date_vec[0].size() == 2 &&
            date_vec[1].size() == 2 &&
            date_vec[2].size() == 4) &&
        (regex_match(date_vec[0], num) &&
            regex_match(date_vec[1], num) &&
            regex_match(date_vec[2], num)) &&
        (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
            in_range(atoi(date_vec[1].c_str()), 1, 12))) {

        object["date"] = argv[3];
    }
    else {
        std::cerr << "Ошибка!\nДата записана в неправильном формате\nФормат записи:    dd.MM.yyyy\n\n";
        return;
    }

    object["description"] = argv[4];

    data["Expense"].push_back(object);

    
    std::ofstream output("data.json");
    if (!output.is_open())
    {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json\n\n";
        return;
    }
    output << data.dump(4);
    output.close();

    std::ifstream inputChek("data.json");
    if (!inputChek.is_open())
    {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json\n\n";
        return;
    }
    if (~input.peek() == EOF) {
        inputChek >> data;
    }
    inputChek.close();
    std::cout << "Покупка успешно добавлена!\n";
    std::cout << "Название: " << data["Expense"].back()["name"] << std::endl;
    std::cout << "Стоимость: " << data["Expense"].back()["price"] << std::endl;
    std::cout << "Дата: " << data["Expense"].back()["date"] << std::endl;
    std::cout << "Описание: " << data["Expense"].back()["description"] << std::endl << std::endl;
}