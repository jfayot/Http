#include "TestServer.hpp"
#include "Http/HttpTypes.hpp"

#include <iostream>

struct titi
{
    std::string coucou;
    int toto;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(titi, coucou, toto)
};

int main()
{
    try
    {
        start_test_server();

        titi toto{"coucou", 42};
        
        Http::Client client{ "http://127.0.0.1:8281" };

        auto res = client.get("/")
            .body(toto)
            .send()
            .get();

        if (true == res.ok())
        {
            std::cout << std::endl << res.body().json() << std::endl;
            auto content = res.body().get<titi>();
            std::cout << content.coucou << "; " << content.toto << std::endl;
        }
        else
        {
            std::cout << res.reason() << std::endl;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << "coucou" << ex.what() << std::endl;
    }
}
