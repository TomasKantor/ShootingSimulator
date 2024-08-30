#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "simulation.cpp"

entt::registry create_registry_with_bullet(){
    entt::registry registry;
    const auto entity = registry.create();
    registry.emplace<position>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<velocity>(entity, 1.0f, 0.0f, 0.0f);
    registry.emplace<acceleration>(entity, 0.0f, 1.0f, 0.0f);
    registry.emplace<mass>(entity, 1.0f);
    return registry;
}

entt::registry create_registry_with_bullet_zero_velocity(){
    entt::registry registry;
    const auto entity = registry.create();
    registry.emplace<position>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<velocity>(entity, 1.0f, 0.0f, 0.0f);
    registry.emplace<acceleration>(entity, 0.0f, -9.0f, 0.0f);
    registry.emplace<mass>(entity, 1.0f);
    return registry;
}

TEST_CASE( "Bullet is created", "[get_bullet_position]" ) {
    entt::registry registry = create_registry_with_bullet();
    position pos = get_bullet_position(registry);
    REQUIRE(pos.x == 0.0f);
    REQUIRE(pos.y == 0.0f);
    REQUIRE(pos.z == 0.0f);
}

TEST_CASE( "Bullet position is updated", "[update_position]" ) {
    entt::registry registry = create_registry_with_bullet();
    update_position(registry, 1.0f);
    position pos = get_bullet_position(registry);
    REQUIRE(pos.x == 1.0f);
    REQUIRE(pos.y == 0.0f);
    REQUIRE(pos.z == 0.0f);
}

TEST_CASE( "Bullet velocity is updated", "[update_velocity]" ) {
    entt::registry registry = create_registry_with_bullet();
    update_velocity(registry, 0.1f);
    auto view = registry.view<velocity>();
    view.each([](auto &vel) {
        REQUIRE(vel.dx == 1.0f);
        REQUIRE(vel.dy == 0.1f);
        REQUIRE(vel.dz == 0.0f);
    });
}

TEST_CASE("Bullet acceleration is updated", "[update_acceleration]") {
    entt::registry registry = create_registry_with_bullet();
    update_acceleration(registry);
    auto view = registry.view<acceleration>();
    view.each([](auto &acc) {
        REQUIRE(acc.ddx < 0); // drag in x direction opposes velocity
        REQUIRE(acc.ddy < -9.0f); // gravity and drag in y direction
        REQUIRE(acc.ddz == 0.0f);
    });
}

TEST_CASE("Bullet x speed goes down", "[update]") {
    entt::registry registry = create_registry_with_bullet();
    float prev_dx = 2.0f;
    float current_dx = 0.0f;

    update(registry, 0.1f);
    auto view = registry.view<velocity>();
    view.each([&current_dx](const auto &vel) {
        current_dx = vel.dx;
    });
    REQUIRE(abs(current_dx) < abs(prev_dx));
    prev_dx = current_dx;

    update(registry, 0.1f);
    view = registry.view<velocity>();
    view.each([&current_dx](const auto &vel) {
        current_dx = vel.dx;
    });
    REQUIRE(abs(current_dx) < abs(prev_dx));
    prev_dx = current_dx;
    
}

TEST_CASE("Bullet fall gets faster", "[update]") {
    entt::registry registry = create_registry_with_bullet_zero_velocity();
    float prev_dy = 0.0f;
    float current_dy = 0.0f;
    update(registry, 0.1f); // compute acceleration
    update(registry, 0.1f); // update speed based on acceleration
    auto view = registry.view<velocity>();
    view.each([&current_dy](const auto &vel) {
        current_dy = vel.dy;
    });
    REQUIRE(abs(current_dy) > abs(prev_dy));

}

TEST_CASE("Distance in one direction", "[get_distance]") {
    position a{0.0f, 0.0f, 0.0f};
    position b{1.0f, 0.0f, 0.0f};
    REQUIRE(get_distance(a, b) == 1.0f);
}

TEST_CASE("Distance in all direction", "[get_distance]") {
    position a{0.0f, 0.0f, 0.0f};
    position b = {1.0f, 1.0f, 1.0f};

    REQUIRE_THAT(get_distance(a, b), Catch::Matchers::WithinAbs(sqrt(3),0.01f));
}

TEST_CASE("Horizontal distance", "[get_horizontal_distance]") {
    position a{0.0f, 0.0f, 0.0f};
    position b{1.0f, 1.0f, -1.0f};
    REQUIRE_THAT(get_horizontal_distance(a, b), Catch::Matchers::WithinAbs(sqrt(2), 0.01f));
}

TEST_CASE("Bullet is behind target", "[is_behind]") {
    position start{0.0f, 0.0f, 0.0f};
    position target{1.0f, 0.0f, 0.0f};
    position pos{2.0f, 0.0f, 0.0f};
    REQUIRE(is_behind(pos, start, target));
}

TEST_CASE("Bullet is not behind target", "[is_behind]"){
    position start{0.0f, 0.0f, 0.0f};
    position target{1.0f, 0.0f, 0.0f};
    position pos{0.5f, 0.0f, 0.0f};
    REQUIRE_FALSE(is_behind(pos, start, target));
}

TEST_CASE("Get launch angle", "[get_launch_angle]") {
    position start{0.0f, 0.0f, 0.0f};
    position target{1.0f, 1.0f, 0.0f};
    // angle should be 45°
    REQUIRE_THAT(get_launch_angle(start, target), Catch::Matchers::WithinAbs(0.785398163f, 0.1f));
}