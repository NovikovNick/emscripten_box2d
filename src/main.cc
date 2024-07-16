
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
  float x, y;
};
struct Box
{
  long id;
  Vector2d position;
  float angle;
  Vector2d bounds;
};

std::unordered_map<long, Box> scene;

constexpr int32 kVelocityIterations = 6;  // suggested 8
constexpr int32 kPositionIterations = 2;  // suggested 3
constexpr float kMaxVelocity = 5.0f;

b2World world{b2Vec2{0.0f, -10.0f}};

int playerId = 100;
b2Body* playerBody;

std::array<Box, 8> groundBodyData;
std::array<b2Body*, 8> groundBody;
}  // namespace

void _start()
{
  // *** data ***
  Box bodyData{playerId, {3.5f, 7.0f}, 0.0f, {0.5f, 1.f}};
  groundBodyData[0] = Box{0, {1.0f, 5.0f}, 0.0f, {1.0f, 5.0f}};
  groundBodyData[1] = Box{1, {5.0f, 1.0f}, 0.0f, {3.0f, 1.0f}};
  groundBodyData[2] = Box{2, {10.0f, 2.0f}, 0.0f, {2.0f, 2.0f}};
  groundBodyData[3] = Box{3, {14.0f, 0.5f}, 0.0f, {2.0f, 0.5f}};
  groundBodyData[4] = Box{4, {18.0f, 3.0f}, 0.0f, {2.0f, 3.0f}};
  groundBodyData[5] = Box{5, {21.0f, 5.0f}, 0.0f, {1.0f, 5.0f}};
  groundBodyData[6] = Box{6, {12.0f, 8.5f}, 0.0f, {2.0f, 0.5f}};
  groundBodyData[7] = Box{7, {10.0f, 12.5f}, 0.0f, {2.0f, 0.5f}};

  scene[bodyData.id] = bodyData;
  for (auto& data : groundBodyData)
  {
    scene[data.id] = data;
  }

  // *** box2d ***
  {
    b2BodyDef definition;
    definition.type = b2_dynamicBody;
    definition.fixedRotation = true;
    definition.position.Set(bodyData.position.x, bodyData.position.y);
    playerBody = world.CreateBody(&definition);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(bodyData.bounds.x, bodyData.bounds.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 1.0f;
    playerBody->CreateFixture(&fixtureDef);
  }

  for (auto& data : groundBodyData)
  {
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(data.position.x, data.position.y);
    groundBody[data.id] = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(data.bounds.x, data.bounds.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &groundBox;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 1.0f;
    groundBody[data.id]->CreateFixture(&fixtureDef);
  }
}

std::vector<Box> _nextFrame(Vector2d force, double deltaTime)
{
  playerBody->ApplyLinearImpulseToCenter({force.x, force.y}, true);

  world.Step(1.0f / 30.0f, kVelocityIterations, kPositionIterations);

  // scene[0] is static: position and angle are always the same
  // scene[0].position.x = groundBody->GetPosition().x;
  // scene[0].position.y = groundBody->GetPosition().y;
  // scene[0].angle = groundBody->GetAngle();

  scene[playerId].position.x = playerBody->GetPosition().x;
  scene[playerId].position.y = playerBody->GetPosition().y;
  scene[playerId].angle = playerBody->GetAngle();

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
      .field("angle", &Box::angle)
      .field("bounds", &Box::bounds);

  function("nextFrame", &_nextFrame);
  function("start", &_start);
}
