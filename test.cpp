#include <catch2/catch_test_macros.hpp>

#include "simulation.cpp"

entt::registry create_registry_with_bullet(){
    entt::registry registry;
    const auto entity = registry.create();
    registry.emplace<position>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<velocity>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<acceleration>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<mass>(entity, 0.0f);
    return registry;
}

TEST_CASE( "Bullet is created", "[factorial]" ) {
    entt::registry registry = create_registry_with_bullet();
    update(registry, 0.01f);
    position pos = get_bullet_position(registry);
    REQUIRE(pos.x == 0.0f);
    REQUIRE(pos.y == 0.0f);
    REQUIRE(pos.z == 0.0f);
}