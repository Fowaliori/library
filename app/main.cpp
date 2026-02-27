#include "../logger/Logger.h"
#include "ThreadSafeQueue.h"

#include <iostream>
#include <thread>

struct LogMessage
{
    std::string text;
    Logger::LogLevel level;
};

Logger::LogLevel parseLevel(const std::string& levelStr)
{
    if (levelStr == "INFO" || levelStr == "info") return Logger::LogLevel::INFO;
    if (levelStr == "WARNING" || levelStr == "warning") return Logger::LogLevel::WARNING;
    if (levelStr == "ERROR" || levelStr == "error") return Logger::LogLevel::ERROR;

    throw std::invalid_argument("Недопустимый уровень важности");
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Использование: ./logger_app <файл> <уровень>\n";
        return 1;
    }

    std::string filename = argv[1];
    Logger::LogLevel defaultLevel;

    try
    {
        defaultLevel = parseLevel(argv[2]);
    }
    catch (...)
    {
        std::cerr << "Неверный уровень. Должно быть INFO, WARNING или ERROR\n";
        return 1;
    }

    try
    {
        Logger logger(filename, defaultLevel);
        ThreadSafeQueue<LogMessage> queue;

        std::thread worker([&]() {
            LogMessage msg;
            while (queue.pop(msg))
            {
                logger.log(msg.text, msg.level);
            }
        });

        while (true)
        {
            std::cout << "\n1) Добавить сообщение\n"
                         "2) Сменить порог\n"
                         "3) Выход\n"
                         "Ввод: ";
            std::string choice;
            std::getline(std::cin, choice);

            if (choice == "3")
                break;

            if (choice == "2")
            {
                std::cout << "Порог: 1=INFO, 2=WARNING, 3=ERROR: ";
                std::string levelChoice;
                std::getline(std::cin, levelChoice);
                try
                {
                    if (levelChoice == "1") defaultLevel = Logger::LogLevel::INFO;
                    else if (levelChoice == "2") defaultLevel = Logger::LogLevel::WARNING;
                    else if (levelChoice == "3") defaultLevel = Logger::LogLevel::ERROR;
                    else throw std::invalid_argument("");
                    logger.setCurrentLevel(defaultLevel);
                    std::cout << "Готово\n";
                }
                catch (...)
                {
                    std::cout << "Неверный выбор\n";
                }
                continue;
            }

            if (choice == "1")
            {
                std::cout << "Сообщение: ";
                std::string message;
                std::getline(std::cin, message);

                std::cout << "Уровень (1=INFO, 2=WARNING, 3=ERROR, Enter=по умолчанию): ";
                std::string levelInput;
                std::getline(std::cin, levelInput);

                Logger::LogLevel level;
                if (levelInput.empty())
                {
                    level = defaultLevel;
                }
                else
                {
                    try
                    {
                        if (levelInput == "1") level = Logger::LogLevel::INFO;
                        else if (levelInput == "2") level = Logger::LogLevel::WARNING;
                        else if (levelInput == "3") level = Logger::LogLevel::ERROR;
                        else throw std::invalid_argument("");
                    }
                    catch (...)
                    {
                        std::cout << "Неверный уровень, беру по умолчанию\n";
                        level = defaultLevel;
                    }
                }

                queue.push({message, level});
                continue;
            }

            std::cout << "Неверный выбор\n";
        }

        queue.push({"Выход", Logger::LogLevel::INFO});
        queue.close();
        worker.join();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
