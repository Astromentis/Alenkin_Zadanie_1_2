#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <windows.h>
using namespace std;
enum Group { WORK, FAMILY, FRIENDS, OTHERS };

struct Date {
    int day, month, year;
};

struct Phone {
    int countryCode;
    int cityCode;
    long long number;
};

struct Contact {
    string lastName, firstName, patronymic;
    Phone phone;
    Date birthDate;
    string email;
    Group category;
};
void clearCin() {
    cin.clear();
    cin.ignore(10000, '\n');
}

int getValidInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            clearCin();
            return value;
        }
        cout << "Ошибка ввода! Введите число от " << minVal << " до " << maxVal << ".\n";
        clearCin();
    }
}

long long getValidLongLong(const string& prompt) {
    long long value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            clearCin();
            return value;
        }
        cout << "Ошибка ввода! Номер не может содержать буквы или быть отрицательным.\n";
        clearCin();
    }
}

bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    if (atPos == string::npos || atPos == 0 || atPos == email.length() - 1) return false;
    size_t dotPos = email.find('.', atPos);
    if (dotPos == string::npos || dotPos == atPos + 1 || dotPos == email.length() - 1) return false;
    return true;
}

string truncateStr(string s, size_t maxLen = 15) {
    if (s.length() > maxLen) {
        return s.substr(0, maxLen - 3) + "...";
    }
    return s;
}

string groupToString(Group g) {
    switch (g) {
    case WORK: return "Work";
    case FAMILY: return "Family";
    case FRIENDS: return "Friends";
    default: return "Others";
    }
}

string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
void addContact(vector<Contact>& db) {
    Contact c;
    cout << "\n--- Добавление контакта ---\n";
    cout << "Фамилия: "; getline(cin, c.lastName);
    cout << "Имя: "; getline(cin, c.firstName);
    cout << "Отчество: "; getline(cin, c.patronymic);

    c.phone.countryCode = getValidInt("Код страны (1-999): ", 1, 999);
    c.phone.cityCode = getValidInt("Код города (1-9999): ", 1, 9999);
    c.phone.number = getValidLongLong("Номер телефона (только цифры): ");

    c.birthDate.day = getValidInt("День рождения (1-31): ", 1, 31);
    c.birthDate.month = getValidInt("Месяц рождения (1-12): ", 1, 12);
    c.birthDate.year = getValidInt("Год рождения (1900-2026): ", 1900, 2026);

    while (true) {
        cout << "Email: ";
        getline(cin, c.email);
        if (isValidEmail(c.email)) break;
        cout << "Некорректный формат почты! Обязателен символ '@' и '.' после него.\n";
    }

    int grp = getValidInt("Группа (0-WORK, 1-FAMILY, 2-FRIENDS, 3-OTHERS): ", 0, 3);
    c.category = static_cast<Group>(grp);

    db.push_back(c);
    cout << "Контакт успешно добавлен!\n";
}

void printTable(const vector<Contact>& db) {
    if (db.empty()) {
        cout << "База контактов пуста.\n";
        return;
    }
    cout << "\n" << string(95, '-') << "\n";
    cout << left << setw(15) << "Фамилия" << " | "
        << setw(15) << "Имя" << " | "
        << setw(18) << "Телефон" << " | "
        << setw(10) << "Дата рожд." << " | "
        << setw(15) << "Email" << " | "
        << "Группа\n";
    cout << string(95, '-') << "\n";

    for (const auto& c : db) {
        string phoneStr = "+" + to_string(c.phone.countryCode) + "(" + to_string(c.phone.cityCode) + ")" + to_string(c.phone.number);
        string dateStr = to_string(c.birthDate.day) + "." + to_string(c.birthDate.month) + "." + to_string(c.birthDate.year);

        cout << left << setw(15) << truncateStr(c.lastName) << " | "
            << setw(15) << truncateStr(c.firstName) << " | "
            << setw(18) << truncateStr(phoneStr, 18) << " | "
            << setw(10) << dateStr << " | "
            << setw(15) << truncateStr(c.email) << " | "
            << groupToString(c.category) << "\n";
    }
    cout << string(95, '-') << "\n";
}

void searchContacts(const vector<Contact>& db) {
    cout << "Введите слова для поиска (через пробел): ";
    string query, word;
    getline(cin, query);
    stringstream ss(toLowerStr(query));
    vector<string> keywords;
    while (ss >> word) keywords.push_back(word);

    vector<Contact> results;
    for (const auto& c : db) {
        string fullData = toLowerStr(c.lastName + " " + c.firstName);
        bool match = true;
        for (const auto& kw : keywords) {
            if (fullData.find(kw) == string::npos) {
                match = false;
                break;
            }
        }
        if (match) results.push_back(c);
    }

    if (results.empty()) cout << "Контакты не найдены.\n";
    else printTable(results);
}

void editContact(vector<Contact>& db) {
    cout << "Введите фамилию контакта для редактирования: ";
    string ln;
    getline(cin, ln);

    vector<int> foundIndices;
    for (size_t i = 0; i < db.size(); ++i) {
        if (toLowerStr(db[i].lastName) == toLowerStr(ln)) {
            foundIndices.push_back(i);
        }
    }

    if (foundIndices.empty()) {
        cout << "Контакт с такой фамилией не найден.\n";
        return;
    }

    int targetIndex = foundIndices[0];
    if (foundIndices.size() > 1) {
        cout << "Найдено несколько контактов:\n";
        for (size_t i = 0; i < foundIndices.size(); ++i) {
            int idx = foundIndices[i];
            cout << i + 1 << ". " << db[idx].lastName << " " << db[idx].firstName
                << " (Email: " << db[idx].email << ")\n";
        }
        int choice = getValidInt("Выберите номер контакта для редактирования: ", 1, foundIndices.size());
        targetIndex = foundIndices[choice - 1];
    }

    Contact& c = db[targetIndex];
    int editChoice;

    do {
        cout << "\n--- Редактирование: " << c.lastName << " " << c.firstName << " ---\n"
            << "1. Изменить ФИО\n"
            << "2. Изменить Телефон\n"
            << "3. Изменить Дату рождения\n"
            << "4. Изменить Email\n"
            << "5. Изменить Группу\n"
            << "0. Сохранить и выйти из редактирования\n"
            << "Что редактируем?: ";
        editChoice = getValidInt("", 0, 5);

        switch (editChoice) {
        case 1:
            cout << "Новая Фамилия: "; getline(cin, c.lastName);
            cout << "Новое Имя: "; getline(cin, c.firstName);
            cout << "Новое Отчество: "; getline(cin, c.patronymic);
            break;
        case 2:
            c.phone.countryCode = getValidInt("Новый код страны (1-999): ", 1, 999);
            c.phone.cityCode = getValidInt("Новый код города (1-9999): ", 1, 9999);
            c.phone.number = getValidLongLong("Новый номер телефона: ");
            break;
        case 3:
            c.birthDate.day = getValidInt("Новый день (1-31): ", 1, 31);
            c.birthDate.month = getValidInt("Новый месяц (1-12): ", 1, 12);
            c.birthDate.year = getValidInt("Новый год (1900-2026): ", 1900, 2026);
            break;
        case 4:
            while (true) {
                cout << "Новый Email: ";
                getline(cin, c.email);
                if (isValidEmail(c.email)) break;
                cout << "Некорректный формат! Обязателен символ '@' и '.' после него.\n";
            }
            break;
        case 5:
            c.category = static_cast<Group>(getValidInt("Новая группа (0-WORK, 1-FAMILY, 2-FRIENDS, 3-OTHERS): ", 0, 3));
            break;
        }
    } while (editChoice != 0);

    cout << "Изменения успешно сохранены!\n";
}

void deleteContact(vector<Contact>& db) {
    cout << "Введите фамилию для удаления: ";
    string ln;
    getline(cin, ln);
    auto it = remove_if(db.begin(), db.end(), [&](const Contact& c) { return toLowerStr(c.lastName) == toLowerStr(ln); });
    if (it != db.end()) {
        db.erase(it, db.end());
        cout << "Контакт(ы) удален(ы).\n";
    }
    else {
        cout << "Контакт не найден.\n";
    }
}
void filterByGroup(const vector<Contact>& db) {
    int grp = getValidInt("Какую группу показать? (0-WORK, 1-FAMILY, 2-FRIENDS, 3-OTHERS): ", 0, 3);
    vector<Contact> results;
    for (const auto& c : db) {
        if (c.category == static_cast<Group>(grp)) results.push_back(c);
    }
    printTable(results);
}

void clearAll(vector<Contact>& db) {
    cout << "Вы уверены? (1 - Да, 0 - Нет): ";
    int choice = getValidInt("", 0, 1);
    if (choice == 1) {
        db.clear();
        cout << "База данных полностью очищена.\n";
    }
}

void exportToCSV(const vector<Contact>& db) {
    if (db.empty()) {
        cout << "База пуста, нечего экспортировать.\n";
        return;
    }
    ofstream out("contacts.csv");
    if (!out.is_open()) {
        cout << "Ошибка при создании файла contacts.csv\n";
        return;
    }
    out << "Фамилия;Имя;Отчество;Код страны;Код города;Номер;День;Месяц;Год;Email;Группа\n";
    for (const auto& c : db) {
        out << c.lastName << ";" << c.firstName << ";" << c.patronymic << ";"
            << c.phone.countryCode << ";" << c.phone.cityCode << ";" << c.phone.number << ";"
            << c.birthDate.day << ";" << c.birthDate.month << ";" << c.birthDate.year << ";"
            << c.email << ";" << groupToString(c.category) << "\n";
    }
    cout << "База успешно экспортирована! Ищи файл contacts.csv в папке с программой.\n";
}

void createBackup(const vector<Contact>& db) {
    ofstream out("backup_database.txt");
    if (!out.is_open()) {
        cout << "Ошибка при создании файла backup_database.txt\n";
        return;
    }
    for (const auto& c : db) {
        out << c.lastName << "|" << c.firstName << "|" << c.patronymic << "|"
            << c.phone.countryCode << "|" << c.phone.cityCode << "|" << c.phone.number << "|"
            << c.birthDate.day << "|" << c.birthDate.month << "|" << c.birthDate.year << "|"
            << c.email << "|" << c.category << "\n";
    }
    cout << "Резервная копия успешно создана (файл backup_database.txt)!\n";
}

void saveToFile(const vector<Contact>& db) {
    ofstream out("database.txt");
    for (const auto& c : db) {
        out << c.lastName << "|" << c.firstName << "|" << c.patronymic << "|"
            << c.phone.countryCode << "|" << c.phone.cityCode << "|" << c.phone.number << "|"
            << c.birthDate.day << "|" << c.birthDate.month << "|" << c.birthDate.year << "|"
            << c.email << "|" << c.category << "\n";
    }
}

void loadFromFile(vector<Contact>& db) {
    ifstream in("database.txt");
    if (!in.is_open()) return;
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string item;
        Contact c;
        getline(ss, c.lastName, '|');
        getline(ss, c.firstName, '|');
        getline(ss, c.patronymic, '|');

        getline(ss, item, '|'); c.phone.countryCode = stoi(item);
        getline(ss, item, '|'); c.phone.cityCode = stoi(item);
        getline(ss, item, '|'); c.phone.number = stoll(item);

        getline(ss, item, '|'); c.birthDate.day = stoi(item);
        getline(ss, item, '|'); c.birthDate.month = stoi(item);
        getline(ss, item, '|'); c.birthDate.year = stoi(item);

        getline(ss, c.email, '|');
        getline(ss, item, '|'); c.category = static_cast<Group>(stoi(item));

        db.push_back(c);
    }
}

void sortContacts(vector<Contact>& db) {
    cout << "1. По алфавиту (Фамилия + Имя)\n2. По дате рождения (Месяц и день)\nВыбор: ";
    int choice = getValidInt("", 1, 2);
    if (choice == 1) {
        sort(db.begin(), db.end(), [](const Contact& a, const Contact& b) {
            return (a.lastName + a.firstName) < (b.lastName + b.firstName);
            });
    }
    else {
        sort(db.begin(), db.end(), [](const Contact& a, const Contact& b) {
            if (a.birthDate.month != b.birthDate.month) return a.birthDate.month < b.birthDate.month;
            return a.birthDate.day < b.birthDate.day;
            });
    }
    cout << "Контакты отсортированы!\n";
}

// --- MAIN ---
int main() {
    vector<Contact> db;
    loadFromFile(db);
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int choice;
    do {
        cout << "\n=== СМАРТ-КОНТАКТЫ ===\n"
            << "1. Добавить контакт\n"
            << "2. Показать все\n"
            << "3. Поиск (по маске)\n"
            << "4. Редактировать контакт\n"
            << "5. Удалить контакт\n"
            << "6. Сортировка\n"
            << "7. Фильтр по группе (Доп. функция 1)\n"
            << "8. Очистить базу (Доп. функция 2)\n"
            << "9. Экспорт в CSV для Excel (Доп. функция 3)\n"
            << "10. Создать бэкап базы (Доп. функция 4)\n"
            << "0. Выход и сохранение\n"
            << "Ваш выбор: ";
        choice = getValidInt("", 0, 10);

        switch (choice) {
        case 1: addContact(db); break;
        case 2: printTable(db); break;
        case 3: searchContacts(db); break;
        case 4: editContact(db); break;
        case 5: deleteContact(db); break;
        case 6: sortContacts(db); break;
        case 7: filterByGroup(db); break;
        case 8: clearAll(db); break;
        case 9: exportToCSV(db); break;
        case 10: createBackup(db); break;
        case 0: saveToFile(db); cout << "Данные сохранены. Пока!\n"; break;
        }
    } while (choice != 0);

    return 0;
}