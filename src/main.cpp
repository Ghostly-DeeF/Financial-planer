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
template<class T>
bool in_range(const T t, T min, T max) {
    return t >= min && t <= max;
}

void help(int argc, std::vector<std::string> argv);
void info(int argc, std::vector<std::string> argv);
void addExpense(int argc, std::vector<std::string> argv);
void editExpense(int argc, std::vector<std::string> argv);

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

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
        {"addExpense", addExpense},
        {"editExpense", editExpense}
        };

        if (map.find(argv[0]) != map.end()) {
            map[argv[0]](argc, argv);
        }
        else {
            std::cout << "Команда не найдена\nВведите help для помощи\n\n";
        }

        argc = 0;
        argv.clear();
    }

    system("pause");
    return 0;
}


void help(int argc, std::vector<std::string> argv) {
    std::cout <<std::setw(20) << std::left << "info" << std::setw(20) << "Получение информации\n";
    std::cout << std::setw(20) << std::left << "addExpense" << std::setw(20) << "Добвление покупки в планировщик\n" << std::endl;
    std::cout << std::setw(20) << std::left << "editExpense" << std::setw(20) << "Редактирование покупки\n" << std::endl;
    
}

void info(int argc, std::vector<std::string> argv) {
    if (argc == 1) {
        std::cout << "Вывод различной информации\n\ninfo [-a] [-e | --expense] [-s | --savings] \n\nОпции\n\t" << std::setw(20) << std::left << "[-a]" << std::setw(20) << "Вся доступная информация\n" << std::setw(20) << "\t[-e | --expense]" << std::setw(20) << " Список всех расходов\n" << std::setw(20) << "\t[-s | --savings]" << std::setw(20) << " Информация о накопления\n\n";
        return;
    }

    if ((std::find(argv.begin(), argv.end(), "-a") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-e") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--expense") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-s") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--savings") == argv.end()))
    {
        std::cerr << "Ошибка!\nНеверные аргументы\n\n";
        return;
    }

    time_t seconds = time(NULL);
    char time_line[31];
    tm* timeinfo = localtime(&seconds);

    strftime(time_line, 31, "Сейчас: %d.%m.%Y Время: %H:%M", timeinfo);
    std::cout << time_line << std::endl;

    if ((std::find(argv.begin(), argv.end(), "-a") != argv.end()) || (std::find(argv.begin(), argv.end(), "--savings") != argv.end()) || (std::find(argv.begin(), argv.end(), "-s") != argv.end())) {
        json data;
        std::ifstream input("data.json");
        if (input.is_open()) {
            if (!(input.peek() == -1)) {
                input >> data;
            }
            input.close();
        }
        else {
            std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
            return;
        }
        
        if (data["Expense"].size() == 0) {
            std::cout << "Покупок не запланированно\n\n";
        }
        else {
            int sum = 0;
            std::cout << "Название" << "\t    " << "Сбережение" << "\t     " << "Остаток" << "\t      " << "Накопленно\n";
            for (int i = 0; i < data["Expense"].size(); i++) {
                std::vector<std::string> start_time{};
                std::vector<std::string> end_time{};
                splitLine(end_time, data["Expense"][i]["date"], '.');
                splitLine(start_time, data["Expense"][i]["start"], '.');

                int mon_ex = ((atoi(end_time[2].c_str()) - atoi(start_time[2].c_str())) * 12) + (atoi(end_time[1].c_str()) - atoi(start_time[1].c_str()));
                
                int mon_now = timeinfo->tm_year * 12 + timeinfo->tm_mon;
                int mon_start = (atoi(start_time[2].c_str()) - 1900) * 12 + atoi(start_time[1].c_str()) - 1;
                int mon_end = (atoi(end_time[2].c_str()) - 1900) * 12 + atoi(end_time[1].c_str()) - 1;

                std::ofstream output("data.json");
                if (!output.is_open())
                {
                    std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
                    return;
                }
                output << data.dump(4);

                if (in_range(mon_now, mon_start, mon_end)) {
                    data["Expense"][i]["accumulated"] = (int)(floor((float)data["Expense"][i]["price"] / mon_ex) * (float)(mon_now - mon_start));
                    std::cout << std::setw(20) << std::left << (std::string)data["Expense"][i]["name"] << std::setw(20) << std::to_string((int)ceil(((float)data["Expense"][i]["price"] / (float)mon_ex))) + " руб" << std::setw(20) << std::to_string((int)data["Expense"][i]["price"] - (int)data["Expense"][i]["accumulated"]) + " руб" << std::setw(20) << std::to_string((int)data["Expense"][i]["accumulated"]) + " руб" << "\n";
                    sum += ceil(((float)data["Expense"][i]["price"] - (float)data["Expense"][i]["accumulated"]) / (float)(mon_ex - (mon_now - mon_start)));
                }

            }
            std::cout << "\nВ этом месяце требуется отложить " << sum << " руб\n\n";
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-a") != argv.end()) || (std::find(argv.begin(), argv.end(), "-e") != argv.end()) || (std::find(argv.begin(), argv.end(), "--expense") != argv.end())) {

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
            std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
            return;
        }

        if (!data.contains("Expense")) {
            std::cout << "Покупок не запланированно\n";
        }
        else {
            std::cout << "Всего запланированно " << data["Expense"].size() << " покупок:\n";
            std::cout << "Название" << "\t    " << "Стоимость" << "\t     " << "Дата покупки" << "\t " << "Дата накопления" << "     " << "Описание\n";
            for (int i = 0; i < data["Expense"].size(); i++) {
                std::cout << std::setw(20) << std::left << (std::string)data["Expense"][i]["name"] << std::setw(20) << (std::to_string((int)data["Expense"][i]["price"]) + " руб") << std::setw(20) << (std::string)data["Expense"][i]["date"] << std::setw(20) << (std::string)data["Expense"][i]["start"] << std::setw(20) << (std::string)data["Expense"][i]["description"] << "\n";
            }
            std::cout << std::endl;
        }
    }

}

void addExpense(int argc, std::vector<std::string> argv) {

    if (argc == 1) {
        std::cout << "Добавление покупки в планировщик\n\naddExpense <name> <price> <date> <description> [-sd | --startdate]\n\nАргументы\n\t<name>\t\t\t\t\tНазвание покупки\n\t<price>\t\t\t\t\tСтоимость покупки\n\t<date>\t\t\t\t\tДата планируемого совершения покупки\n\t<description>\t\t\t\tОписание покупки\n\nОпции\n\t[-sd | --startdate] <start date>\tДата, с которой вы планируете начать копить\n\n";
        return;
    }
    if (argc < 5) {
        std::cerr << "Ошибка!\nнедостаточно аргументов\n\n";
        return;
    }

    for (int i = 5; i < argc; i++) {
        if (argv[5][0] == '-')
            break;
        argv[4] += (" " + argv[5]);
        argv.erase(argv.begin() + 5);
    }

    if (((std::find(argv.begin() + 5, argv.end(), "-sd") == argv.end()) &&
        (std::find(argv.begin() + 5, argv.end(), "--startdate") == argv.end())) &&
        (argv.size() > 5))
    {
        std::cerr << "Ошибка!\nНеправильные опции\n\n";
        return;
    }

    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);

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
            std::cout << "Покупка " << argv[1] << " уже существует!\nЕсли вы хотите изменить существующую покупку то используйте команду editExpense\n\n";
            return;
        }
    }
    

    json object;

    object["name"] = argv[1];

    if (regex_match(argv[2], num)) {
        object["price"] = atoi(argv[2].c_str());
    }
    else {
        std::cerr << "Ошибка!\nСтоимость должна состоять только из цифр\n\n";
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
            in_range(atoi(date_vec[1].c_str()), 1, 12)) &&
        ((atoi(date_vec[2].c_str()) - timeinfo->tm_year - 1900) * 12) + (atoi(date_vec[1].c_str()) - 1 - timeinfo->tm_mon) >= 1)
    {

        object["date"] = argv[3];
    }
    else {
        std::cerr << "Ошибка!\nДата введена не правильно\nДата должна быть в будущем минимум на 1 месяц\nФормат даты:    dd.mm.yyyy\n\n";
        return;
    }

    object["description"] = argv[4];

    if ((std::find(++argv.begin(), argv.end(), "-sd") != argv.end()) || (std::find(++argv.begin(), argv.end(), "--startdate") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 5, argv.end(), "-sd") != argv.end())) {

            iter = std::find(argv.begin() + 5, argv.end(), "-sd");
        }
        else {
            iter = std::find(argv.begin() + 5, argv.end(), "--startdate");
        }
        if (iter == --argv.end()) {
            std::cerr << "Ошибка!\nВведите дату начала накопления\n\n";
            return;
        }

        std::string start_date = *(++iter);

        date_vec.clear();
        splitLine(date_vec, start_date, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)))
        {

            object["start"] = start_date;
            
        }
        else {
            std::cerr << "Ошибка!\nДата введена не правильно\nФормат даты:    dd.mm.yyyy\n\n";
            return;
        }
    }
    else {
        char time_line[11]{};
        strftime(time_line, 11, "%d.%m.%Y", timeinfo);
        object["start"] = time_line;
    }

    object["accumulated"] = 0;
    object["custom_coef"] = 0;

    data["Expense"].push_back(object);

    
    std::ofstream output("data.json");
    if (!output.is_open())
    {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
        return;
    }
    output << data.dump(4);
    output.close();

    std::ifstream inputChek("data.json");
    if (!inputChek.is_open())
    {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
        return;
    }
    if (!(input.peek() == -1)) {
        inputChek >> data;
    }
    inputChek.close();
    std::cout << "Покупка добавлена!\n";
    std::cout << "Название:\t\t" << (std::string)data["Expense"].back()["name"] << std::endl;
    std::cout << "Стоимость:\t\t" << data["Expense"].back()["price"] << std::endl;
    std::cout << "Дата покупки:\t\t" << (std::string)data["Expense"].back()["date"] << std::endl;
    std::cout << "Старт накопления:\t" << (std::string)data["Expense"].back()["start"] << std::endl;
    std::cout << "Описание:\t\t" << (std::string)data["Expense"].back()["description"] << std::endl << std::endl;
}

void editExpense(int argc, std::vector<std::string> argv) {
    if (argc == 1) {
        std::cout << "Редактирование покупки\n\editExpense <name> [-n | --name] [-p | --price] [-t | --time] [-d | --description] [-sd | --startdate] [-ac | --accumulated] <args> \n\nОпции\n\t" << std::setw(20) << std::left << "[-a]" << std::setw(20) << "Вся доступная информация\n" << std::setw(20) << "\t[-e | --expense]" << std::setw(20) << " Список всех расходов\n" << std::setw(20) << "\t[-s | --savings]" << std::setw(20) << " Информация о накопления\n\n";
        return;
    }

    if (argc < 2) {
        std::cerr << "Ошибка!\nНедостаточно аргументов\n\n";
        return;
    }

    if ((std::find(argv.begin(), argv.end(), "-p") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "---price") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-t") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--time") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-d") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--description") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-sd") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--startdate") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-ac") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--accumulated") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "-n") == argv.end()) &&
        (std::find(argv.begin(), argv.end(), "--name") == argv.end()))
    {
        std::cerr << "Ошибка!\nНеверные аргументы\n\n";
        return;
    }

    json data;
    std::ifstream input("data.json");
    if (input.is_open()) {
        if (!(input.peek() == -1)) {
            input >> data;
        }
        input.close();
    }
    else {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
        return;
    }

    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    std::regex num("[0-9-]*");

    if ((std::find(argv.begin(), argv.end(), "-p") != argv.end()) || (std::find(argv.begin(), argv.end(), "--price") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-p") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-p");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--price");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новую стоимость\n";
            return;
        }
        --iter;

        std::string new_price = *(++iter);

        if (!regex_match(new_price, num)) {
            std::cerr << "Ошибка!\nСтоимость должна состоять только из цифр\n\n";
            return;
        }

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                int old_price = data["Expense"][i]["price"];
                data["Expense"][i]["price"] = atoi(new_price.c_str());
                std::cout << "Стоимость покупки " << argv[1] << " успешно изменена\n" << old_price << " -> " << new_price << "\n";
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
            return;
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-t") != argv.end()) || (std::find(argv.begin(), argv.end(), "--time") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-t") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-t");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--time");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новую дату покупки\n";
            return;
        }
        --iter;

        std::string new_time = *(++iter);

        std::vector<std::string> date_vec{};
        splitLine(date_vec, new_time, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)) &&
            ((atoi(date_vec[2].c_str()) - timeinfo->tm_year - 1900) * 12) + (atoi(date_vec[1].c_str()) - 1 - timeinfo->tm_mon) >= 1)
        {

            for (int i = 0; i < data["Expense"].size(); i++) {
                if (data["Expense"][i]["name"] == argv[1]) {
                    std::string old_time = (std::string)data["Expense"][i]["date"];
                    data["Expense"][i]["date"] = new_time;
                    std::cout << "Дата покупки " << argv[1] << " успешно изменена\n" << old_time << " -> " << new_time << "\n";
                    break;
                }
                else if (i != data["Expense"].size() - 1) {
                    continue;
                }
                std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
                return;
            }

        }
        else {
            std::cerr << "Ошибка!\nДата введена не правильно\nДата должна быть в будущем минимум на 1 месяц\nФормат даты:    dd.mm.yyyy\n\n";
            return;
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-d") != argv.end()) || (std::find(argv.begin(), argv.end(), "--description") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-d") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-d");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--description");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новое описание\n";
            return;
        }
        --iter;

        std::vector<std::string>::iterator unite_iter = iter;

        while (unite_iter != --argv.end()) {
            if ((*(++unite_iter))[0] != '-') {
                *(--unite_iter) += " " + *(unite_iter);
                ++unite_iter;
            }
            else {
                break;
            }
        }

        std::string new_desc = *(++iter);

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                std::string old_desc = (std::string)data["Expense"][i]["description"];
                data["Expense"][i]["description"] = new_desc;
                std::cout << "Описание покупки " << argv[1] << " успешно изменено\n" << old_desc << " -> " << new_desc << "\n";
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
            return;
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-sd") != argv.end()) || (std::find(argv.begin(), argv.end(), "--startdate") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-sd") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-sd");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--startdate");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новую дату покупки\n";
            return;
        }
        --iter;

        std::string new_date = *(++iter);

        std::vector<std::string> date_vec{};
        splitLine(date_vec, new_date, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)))
        {

            for (int i = 0; i < data["Expense"].size(); i++) {
                if (data["Expense"][i]["name"] == argv[1]) {
                    std::string old_date = (std::string)data["Expense"][i]["start"];
                    data["Expense"][i]["start"] = new_date;
                    std::cout << "Дата начала накопления покупки " << argv[1] << " успешно изменена\n" << old_date << " -> " << new_date << "\n";
                    break;
                }
                else if (i != data["Expense"].size() - 1) {
                    continue;
                }
                std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
                return;
            }

        }
        else {
            std::cerr << "Ошибка!\nДата введена не правильно\nФормат даты:    dd.mm.yyyy\n\n";
            return;
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-ac") != argv.end()) || (std::find(argv.begin(), argv.end(), "--accumulated") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-ac") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-ac");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--accumulated");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новую стоимость\n";
            return;
        }
        --iter;

        std::string new_accumulated = *(++iter);

        if (!regex_match(new_accumulated, num)) {
            std::cerr << "Ошибка!\nСтоимость должна состоять только из цифр\n\n";
            return;
        }

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                int old_accumulated = (int)data["Expense"][i]["accumulated"] + (int)data["Expense"][i]["custom_coef"];
                data["Expense"][i]["custom_coef"] = (int)data["Expense"][i]["custom_coef"] + atoi(new_accumulated.c_str());
                std::cout << "Накопленная сумма покупки " << argv[1] << " успешно изменена\n" << old_accumulated << " -> " << (int)data["Expense"][i]["accumulated"] + (int)data["Expense"][i]["custom_coef"] << "\n";
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
            return;
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-n") != argv.end()) || (std::find(argv.begin(), argv.end(), "--name") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-n") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-n");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--name");
        }
        if (iter == --argv.end() || (*(++iter))[0] == '-') {
            std::cerr << "Ошибка!\nВведите новое название\n";
            return;
        }
        --iter;

        std::string new_name = *(++iter);

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                data["Expense"][i]["name"] = new_name;
                std::cout << "Покупка " << argv[1] << " успешно переименована\n" << argv[1] << " -> " << new_name << "\n";
                break;
            }
            else if (i != data["Expense"].size() - 1){
                continue;
            }
            std::cout << "Ошибка!\nТакой покупки не существует!\n\n";
            return;
        }
    }

    std::ofstream output("data.json");
    if (!output.is_open())
    {
        std::cerr << "Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n";
        return;
    }
    output << data.dump(4);
    output.close();

    std::cout << std::endl;
}