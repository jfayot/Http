#include "EchoServer.hpp"
#include "Http/HttpTypes.hpp"

#include <gtest/gtest.h>

#include <memory>

struct Person
{
  std::string name;
  int age;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, name, age)
  friend bool operator==(const Person& l, const Person& r)
  {
    return l.name == r.name && l.age == r.age;
  }
};

class HttpFixture : public ::testing::Test
{
  virtual void SetUp() override
  {
    server = std::make_unique<EchoServer>();
    server->start();
    client = std::make_unique<Http::Client>("http://127.0.0.1:8281");
  }

  virtual void TearDown() override
  {
    server->stop();
    server.release();
    client.release();
  }

protected:
  std::unique_ptr<EchoServer> server;
  std::unique_ptr<Http::Client> client;
};

TEST_F(HttpFixture, test_send_json)
{
  Person person{"captain", 42};
  auto res = client->get("/").body(person).send().get();
  ASSERT_TRUE(res.ok());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, person);
}
