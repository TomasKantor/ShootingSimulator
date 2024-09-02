#include <iostream>
#include <cmath>
#include <vector>

#include "entt/entt.hpp"

const float GRAVITY = 9.8;
const float AIR_DENSITY = 1.225; // at 15 degrees Celsius and 1 atm
const float DRAG_COEFFICIENT = 0.47; // for a sphere
const float BULLET_AREA = 0.0005067; // 7.62 mm bullet
const float RADIAN_TO_DEGREE = 180.0/3.14159265359;

const int MAX_ITERATIONS = 10000;
const int MAX_SHOTS = 1;

struct position {
    float x;
    float y;
    float z;
};

struct velocity {
    float dx;
    float dy;
    float dz;
};

struct acceleration {
    float ddx;
    float ddy;
    float ddz;
};

struct mass {
    float value;
};

struct drag_coefficient {
    float value;
};


/// @brief Update acceleration based on drag force and gravity
/// @param registry entt registry containing bullet
void update_acceleration(entt::registry &registry) {
    auto view = registry.view<acceleration, const mass, const velocity>();

    view.each([](auto &acc, const auto &mass, const auto &vel) {
        // calculate total velocity
        float velocity = sqrt(vel.dx * vel.dx + vel.dy * vel.dy + vel.dz * vel.dz);
        // calculate total drag force
        float drag = velocity * velocity * AIR_DENSITY * BULLET_AREA * DRAG_COEFFICIENT / (2*mass.value);

        // calculate drag force components
        float drag_x = drag * vel.dx / velocity;
        float drag_y = drag * vel.dy / velocity;
        float drag_z = drag * vel.dz / velocity;

        // apply drag force in opposite direction
        acc.ddx = -drag_x;
        acc.ddy = -drag_y - GRAVITY;
        acc.ddz = -drag_z;
    });
}


/// @brief Update velocity based on acceleration
/// @param registry entt registry containing bullet
/// @param dt time step in seconds
void update_velocity(entt::registry &registry, float dt) {
    auto view = registry.view<velocity, const acceleration>();

    
    view.each([&dt](auto &vel, const auto &acc) {
        // update velocity based on acceleration
        vel.dx += acc.ddx*dt;
        vel.dy += acc.ddy*dt;
        vel.dz += acc.ddz*dt;
    });
}


/// @brief Update position based on velocity
/// @param registry entt registry containing bullet
/// @param dt time step in seconds
void update_position(entt::registry &registry, float dt) {
    auto view = registry.view<position, const velocity>();

    view.each([&dt](auto &pos,const auto &vel) {
        // update position based on velocity
        pos.x += vel.dx * dt;
        pos.y += vel.dy * dt;
        pos.z += vel.dz * dt;
    });
}


/// @brief Print position and velocity of bullet in csv format
/// @param registry entt registry containing bullet
void print_position_and_velocity(entt::registry &registry) {
    auto view = registry.view<const position, const velocity>();

    view.each([](const auto &pos, const auto &vel) {
        std::cout <<  pos.x << ", " << pos.y << ", " << pos.z << ",     " << vel.dx << ", " <<vel.dy << ", " << vel.dz  << std::endl;
    });
}


/// @brief Get bullet position
/// @param registry entt registry containing bullet
/// @return bullet position
position get_bullet_position(entt::registry &registry) {
    position pos;
    auto view = registry.view<const position>();

    for(auto entity : view) {
        pos = view.get<const position>(entity);
    }
    return pos;
}

/// @brief Update bullet position, velocity and acceleration
/// @param registry entt registry containing bullet
/// @param dt time step in seconds
/// @return current bullet position
position update(entt::registry &registry, float dt) {
    update_velocity(registry, dt);
    update_position(registry, dt);
    update_acceleration(registry);
    return get_bullet_position(registry);
}


/// @brief Pythoagorean distance between two positions
/// @param a position a
/// @param b position b
/// @return pythagorean distance
float get_distance(position a, position b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}


/// @brief Horizontal pythagorean distance between two positions
/// @param a position a
/// @param b position b
/// @return horizontal pythagorean distance
float get_horizontal_distance(position a, position b) {
    float dx = b.x - a.x;
    float dz = b.z - a.z;
    return sqrt(dx*dx + dz*dz);
}


/// @brief Check if position is behind a target
/// @param pos bullet position
/// @param start starting position
/// @param target target position
/// @return true if position is behind target
bool is_behind(position pos, position start, position target){
    // check if bullet is behind target in x or z axis
    // check if current position is further away than at the start
    bool behind_x = abs(target.x - start.x) < abs(pos.x - start.x);
    bool behind_z = abs(target.z - start.z) < abs(pos.z - start.z);

    return behind_x || behind_z;
}


/// @brief Get optimal horizontal velocity to hit target assuming no air drag
/// @param start starting position
/// @param aim position to aim at
/// @param velocity velocity of the bullet at the start
/// @return optiomal horizontal velocity
float get_optimal_horizontal_velocity(position start, position aim, float velocity) {
    float sh = get_horizontal_distance(start, aim);
    float sv = aim.y - start.y;
    float sv2 = sv*sv;
    float sh2 = sh*sh;
    float sh4 = sh2*sh2;
    float a = sv2 + sh2;
    float b = -sv*sh2*(-GRAVITY) -velocity*velocity*sh2;
    float c = GRAVITY*GRAVITY*sh4/4;

    float discriminant = b*b - 4*a*c;
    if(discriminant < 0) {
        return -1;
    }
    float v1 = (-b + sqrt(discriminant))/(2*a);
    float v2 = (-b - sqrt(discriminant))/(2*a);
    std::cout << "v1: " << sqrt(v1) << " v2: " << sqrt(v2) << std::endl;
    float v = std::max(v1, v2);
    
    if(v > 0) {
        return sqrt(v);
    }
    return -1;
}

/// @brief Get optimal vertical velocity to hit target assuming no air drag, with known horizontal velocity
/// @param start starting position
/// @param aim position to aim at
/// @param horizontal_velocity horizontal velocity of the bullet
/// @return optimal vertical velocity
float get_optimal_vertical_velocity(position start, position aim, float horizontal_velocity) {
    float sh = get_horizontal_distance(start, aim);
    std::cout << "v: " << horizontal_velocity << std::endl;
    std::cout << "sh: " << sh << std::endl;
    float t = std::abs(sh/horizontal_velocity);
    float sv = aim.y - start.y;
    float v = (sv + 0.5*GRAVITY*t*t)/t;
    return v;
}


/// @brief Get velocty vector to aim strait at target
/// @param start starting position
/// @param aim position to aim at
/// @param velocity velocity of the bullet at the start
/// @return velocity vector
velocity aim_strait(position start, position aim, float velocity) {
    float dx = aim.x - start.x;
    float dy = aim.y - start.y;
    float dz = aim.z - start.z;
    float distance = sqrt(dx*dx + dy*dy + dz*dz);
    float vx = velocity*dx/distance;
    float vy = velocity*dy/distance;
    float vz = velocity*dz/distance;
    return {vx, vy, vz};
}

/// @brief Get velocty vector to hit target assuming gravity and no air drag
/// @param start starting position
/// @param aim position to aim at
/// @param velocity velocity of the bullet at the start
/// @return velocity vector
velocity aim_with_gravity(position start, position aim, float velocity){
    float vh = get_optimal_horizontal_velocity(start, aim, velocity);
    float vy = get_optimal_vertical_velocity(start, aim, vh);
    float dx = aim.x - start.x;
    float dz = aim.z - start.z;
    float dh = sqrt(dx*dx + dz*dz);
    float vx = vh*dx/dh;
    float vz = vh*dz/dh;
    return {vx, vy, vz};
}



/// @brief Simulates bullet trajectory
/// @param start starting position
/// @param aim point to aim at, not necessarily the target
/// @param dt time step in seconds
/// @param bullet_mass mass of the bullet 
/// @param velocity_init initial velocity of the bullet
/// @return closest horizontal position to target
position simulate(position start, position aim, float dt, float bullet_mass, velocity vel, std::vector<position> * history) {
    entt::registry registry;

    // create bullet entity and add components
    const auto entity = registry.create();
    registry.emplace<position>(entity, start.x, start.y, start.z);
    registry.emplace<velocity>(entity, vel.dx, vel.dy, vel.dz);
    registry.emplace<acceleration>(entity, 0.0f, -GRAVITY, 0.0f);
    registry.emplace<mass>(entity, bullet_mass);


    float min_distance = get_horizontal_distance(start, aim);
    position closest_position = start;
    position current_position;

    // update bullet position until it is behind the target
    for(int i = 0; i < MAX_ITERATIONS; i++) {
        current_position = update(registry, dt);
        if(history != nullptr) {
            history->push_back(current_position);
        }
        if(is_behind(current_position, start, aim)) {
            break;
        }
        // udpate closest horizontal position
        float current_distance = get_horizontal_distance(current_position, aim);
        if(current_distance < min_distance) {
            min_distance = current_distance;
            closest_position = current_position;
        }
    }
    return closest_position;
}


/// @brief Get angle between start and target
/// @param start starting position
/// @param target target position
/// @return angle in radians
float get_launch_angle(position start, position target){
    float horizontal_distance = get_horizontal_distance(start, target);
    float vertical_distance = target.y - start.y;
    return atan(vertical_distance / horizontal_distance);
}


/// @brief Get angle of velocity vector
/// @param vel 
/// @return angle in radians
float get_launch_angle(velocity vel){
    float horizontal_velocity = sqrt(vel.dx*vel.dx + vel.dz*vel.dz);
    float vertical_velocity = vel.dy;
    return atan(vertical_velocity / horizontal_velocity);
}
