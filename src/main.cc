
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

#include <map>
#include <string>
#include <unordered_map>
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

using namespace emscripten;

namespace
{

struct Vector2d
{
  double x, y;
};
struct Box
{
  long id;
  Vector2d position;
  Vector2d bounds;
};

std::unordered_map<long, Box> scene;

constexpr int32 kVelocityIterations = 6;  // suggested 8
constexpr int32 kPositionIterations = 2;  // suggested 3

b2World world{b2Vec2{0.0f, -10.0f}};
b2Body* body;
b2Body* groundBody;
}  // namespace

void _start()
{
  // *** data ***

  Box groundBodyData{0, {0.0f, -10.0f}, {50.0f, 10.0f}};
  Box bodyData{1, {0.0f, 40.0f}, {1.0f, 1.0f}};
  scene[groundBodyData.id] = groundBodyData;
  scene[bodyData.id] = bodyData;

  // *** box2d ***

  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(groundBodyData.position.x, groundBodyData.position.y);

  groundBody = world.CreateBody(&groundBodyDef);

  b2PolygonShape groundBox;
  groundBox.SetAsBox(groundBodyData.bounds.x, groundBodyData.bounds.y);
  groundBody->CreateFixture(&groundBox, 0.0f);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(bodyData.position.x, bodyData.position.y);
  body = world.CreateBody(&bodyDef);

  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(bodyData.bounds.x, bodyData.bounds.y);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  body->CreateFixture(&fixtureDef);
}

std::vector<Box> _nextFrame(double deltaTime)
{
  world.Step(deltaTime, kVelocityIterations, kPositionIterations);
  b2Vec2 position = body->GetPosition();
  // float angle = body->GetAngle();
  scene[0].position.x = groundBody->GetPosition().x;
  scene[0].position.y = groundBody->GetPosition().y;

  scene[1].position.x = body->GetPosition().x;
  scene[1].position.y = body->GetPosition().y;

  std::vector<Box> res;
  for (const auto& [id, box] : scene)
  {
    res.push_back(box);
  }
  return res;
}

EMSCRIPTEN_BINDINGS(embind)
{
  register_vector<Box>("BoxVector");

  value_object<Vector2d>("Vector2d")
      .field("x", &Vector2d::x)
      .field("y", &Vector2d::y);

  value_object<Box>("Box")
      .field("id", &Box::id)
      .field("position", &Box::position)
      .field("bounds", &Box::bounds);

  function("nextFrame", &_nextFrame);
  function("start", &_start);
}
