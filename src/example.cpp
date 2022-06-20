#include "EchoServer.hpp"
#include "Http/HttpTypes.hpp"

#include <iostream>
#include <cassert>

struct Person
{
    std::string name;
    int age;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, name, age)
};

int main()
{
    try
    {
        start_echo_server();

        Person person{"captain", 42};

        Http::Client client{ "http://127.0.0.1:8281" };

        auto res = client.get("/")
            .body(person)
            .send()
            .get();

        if (true == res.ok())
        {
            std::cout << res.body().json() << std::endl;
            auto content = res.body().get<Person>();
            assert(person == content);
        }
        else
        {
            std::cout << res.reason() << std::endl;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
