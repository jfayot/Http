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

TEST_F(HttpFixture, test_get_empty)
{
  auto res = client->get("/").send().get();
  ASSERT_TRUE(res.ok());
  ASSERT_TRUE(res.body().isEmpty());
}

TEST_F(HttpFixture, test_get_string)
{
  std::string body("hello world");
  auto res = client->get("/").body(body).send().get();
  ASSERT_TRUE(res.ok());
  ASSERT_TRUE(res.body().isText());
  auto content = res.body().text();
  ASSERT_EQ(content, body);
}

TEST_F(HttpFixture, test_get_json)
{
  nlohmann::json body = {
    {"name", "captain"},
    {"age", 42}
  };
  auto res = client->get("/").body(body).send().get();
  ASSERT_TRUE(res.ok());
  ASSERT_TRUE(res.body().isJson());
  auto content = res.body().json();
  ASSERT_EQ(content, body);
}

TEST_F(HttpFixture, test_get_struct)
{
  Person body{"captain", 42};
  auto res = client->get("/").body(body).send().get();
  ASSERT_TRUE(res.ok());
  ASSERT_TRUE(res.body().isJson());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, body);
}

TEST_F(HttpFixture, test_post_struct)
{
  Person person{"captain", 42};
  auto res = client->post("/").body(person).send().get();
  ASSERT_TRUE(res.ok());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, person);
}

TEST_F(HttpFixture, test_put_struct)
{
  Person person{"captain", 42};
  auto res = client->put("/").body(person).send().get();
  ASSERT_TRUE(res.ok());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, person);
}

TEST_F(HttpFixture, test_patch_struct)
{
  Person person{"captain", 42};
  auto res = client->patch("/").body(person).send().get();
  ASSERT_TRUE(res.ok());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, person);
}

TEST_F(HttpFixture, test_delete_struct)
{
  Person person{"captain", 42};
  auto res = client->delete_("/").body(person).send().get();
  ASSERT_TRUE(res.ok());
  auto content = res.body().get<Person>();
  ASSERT_EQ(content, person);
}
